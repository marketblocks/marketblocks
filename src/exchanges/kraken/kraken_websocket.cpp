#include "kraken_websocket.h"
#include "logging/logger.h"
#include "common/utils/containerutils.h"
#include "common/utils/stringutils.h"
#include "common/exceptions/mb_exception.h"
#include "exchanges/exchange_ids.h"

namespace
{
	using namespace mb;

	static std::unordered_map<websocket_channel, std::string_view> CHANNEL_NAMES
	{
		{ websocket_channel::PRICE, "trade" },
		{ websocket_channel::OHLCV, "ohlc" },
		{ websocket_channel::ORDER_BOOK, "book" }
	};

	static constexpr const char PAIR_SEPARATOR = '/';

	int get_ohlc_interval(ohlcv_interval interval)
	{
		switch (interval)
		{
		case ohlcv_interval::M1:
			return 1;
		case ohlcv_interval::M5:
			return 5;
		case ohlcv_interval::M15:
			return 15;
		case ohlcv_interval::H1:
			return 60;
		case ohlcv_interval::D1:
			return 1440;
		case ohlcv_interval::W1:
			return 10080;
		default:
			throw mb_exception{ std::format("OHLCV interval not supported on Kraken") };
		}
	}

	template<typename Subscription>
	std::string_view get_full_channel_name(const Subscription& subscription)
	{
		std::string channel{ CHANNEL_NAMES[subscription.channel()] };

		if (subscription.channel() == websocket_channel::OHLCV)
		{
			channel += "-" + std::to_string(get_ohlc_interval(subscription.get_ohlcv_interval()));
		}

		return channel;
	}

	std::string generate_subscription_id(std::string symbol, std::string channelName)
	{
		return symbol + channelName;
	}

	std::string create_message(std::string eventName, const websocket_subscription& subscription)
	{
		std::vector<std::string> tradablePairList{ to_vector<std::string>(subscription.pair_item(), 
			[](const tradable_pair& pair) { return pair.to_string(PAIR_SEPARATOR); })};

		json_writer subscriptionJson;
		subscriptionJson.add("name", CHANNEL_NAMES[subscription.channel()]);

		if (subscription.channel() == websocket_channel::OHLCV)
		{
			subscriptionJson.add("interval", get_ohlc_interval(subscription.get_ohlcv_interval()));
		}
		
		return json_writer{}
			.add("event", eventName)
			.add("pair", std::move(tradablePairList))
			.add("subscription", subscriptionJson.to_json())
			.to_string();
	}

	/*order_book_side get_order_book_side(const std::string sideId)
	{
		return sideId.contains("a") 
			? order_book_side::ASK
			: order_book_side::BID;
	}

	order_book_entry create_order_book_entry(order_book_side side, const json_element& json)
	{
		return order_book_entry
		{
			std::stod(json.get<std::string>(0)),
			std::stod(json.get<std::string>(1)),
			side
		};
	}

	order_book_cache create_order_book_cache(const json_element& json)
	{
		json_element asks = json.element("as");
		json_element bids = json.element("bs");

		int depth = asks.size();

		ask_cache askCache;
		bid_cache bidCache;

		for (int i = 0; i < depth; ++i)
		{
			askCache.emplace(create_order_book_entry(order_book_side::ASK, asks.element(i)));
			bidCache.emplace(create_order_book_entry(order_book_side::BID, bids.element(i)));
		}

		return order_book_cache{ std::move(askCache), std::move(bidCache) };
	}

	std::vector<order_book_entry> create_order_book_entries(const json_element& json)
	{
		json_iterator first = json.begin();
		std::string entrySideId = first.key();
		json_element element = first.value();

		order_book_side side = get_order_book_side(entrySideId);
		bool replace = false;
		std::string priceToReplace;

		for (auto entryIterator = element.begin(); entryIterator != element.end(); ++entryIterator)
		{
			json_element entryJson{ entryIterator.value() };
			order_book_cache_entry cacheEntry{ create_order_book_cache_entry(side, entryJson) };

			if (cacheEntry.entry.volume() == 0.0)
			{
				replace = true;
				priceToReplace = std::move(cacheEntry.price);
				continue;
			}

			if (replace)
			{
				localOrderBook.replace_in_book(pair, std::move(priceToReplace), std::move(cacheEntry));
			}
			else
			{
				localOrderBook.update_book(pair, std::move(cacheEntry));
			}
		}
	}*/
}

namespace mb::internal
{
	kraken_websocket_stream::kraken_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory)
		: exchange_websocket_stream{ exchange_ids::KRAKEN, URL, std::move(connectionFactory) }
	{}

	std::string kraken_websocket_stream::generate_subscription_id(const unique_websocket_subscription& subscription) const
	{
		std::string pairName{ subscription.pair_item().to_string(PAIR_SEPARATOR) };
		std::string channelName{ get_full_channel_name(subscription) };

		return ::generate_subscription_id(std::move(pairName), std::move(channelName));
	}

	void kraken_websocket_stream::set_sub_status(const websocket_subscription& subscription, subscription_status status)
	{
		std::string channelName{ get_full_channel_name(subscription) };

		for (auto& pair : subscription.pair_item())
		{
			std::string subId{ ::generate_subscription_id(pair.to_string(PAIR_SEPARATOR), channelName) };
			update_subscription_status(subId, subscription.channel(), status);
		}
	}

	void kraken_websocket_stream::process_event_message(const json_document& json)
	{
		// TODO
	}

	void kraken_websocket_stream::process_price_message(std::string subscriptionId, const json_document& json)
	{
		json_element tradesArray{ json.element(1) };
		json_element lastTrade{ tradesArray.element(tradesArray.size() - 1) };
		double price{ std::stod(lastTrade.get<std::string>(0)) };

		update_price(std::move(subscriptionId), price);
	}

	void kraken_websocket_stream::process_ohlcv_message(std::string subscriptionId, const json_document& json)
	{
		json_element ohlcArray{ json.element(1) };

		update_ohlcv(std::move(subscriptionId), ohlcv_data
			{
				std::stod(ohlcArray.get<std::string>(2)),
				std::stod(ohlcArray.get<std::string>(3)),
				std::stod(ohlcArray.get<std::string>(4)),
				std::stod(ohlcArray.get<std::string>(5)),
				std::stod(ohlcArray.get<std::string>(7))
			});
	}

	/*void kraken_websocket_stream::process_order_book_message(std::string subscriptionId, const json_document& json)
	{
		if (json.size() == 4)
		{
			json_element entryObject = json.element(1);

			if (entryObject.has_member("as"))
			{
				order_book_cache cache = create_order_book_cache(entryObject);
				initialise_order_book(std::move(subscriptionId), std::move(cache));
			}
			else
			{
				std::vector<order_book_entry> entries{ create_order_book_entries(std::move(entryObject)) };
			}
		}
		else
		{
			std::vector<order_book_entry> entries{ create_order_book_entries(std::move(entryObject)) };
			process_order_book_object(pair, localOrderBook, json.element(1));
			process_order_book_object(pair, localOrderBook, json.element(2));
		}
	}*/

	void kraken_websocket_stream::on_message(std::string_view message)
	{
		json_document json{ parse_json(message) };
		json_value_type type = json.type();

		if (json.type() == json_value_type::ARRAY)
		{
			std::string channelName{ json.element(json.size() - 2).get<std::string>() };
			std::string pairName{ json.element(json.size() - 1).get<std::string>() };
			std::string subscriptionId{ ::generate_subscription_id(pairName, channelName) };

			if (channelName == "trade")
			{
				process_price_message(std::move(subscriptionId), json);
			}
			else if (channelName.contains("ohlc"))
			{
				process_ohlcv_message(std::move(subscriptionId), json);
			}
			/*if (channelName.contains("book"))
			{
				process_order_book_message(std::move(subscriptionId), json);
			}*/
		}
		else
		{
			process_event_message(json);
		}
	}

	void kraken_websocket_stream::subscribe(const websocket_subscription& subscription)
	{
		std::string message{ create_message("subscribe", subscription) };

		set_sub_status(subscription, subscription_status::SUBSCRIBED);

		_connection->send_message(message);
	}

	void kraken_websocket_stream::unsubscribe(const websocket_subscription& subscription)
	{
		std::string message{ create_message("unsubscribe", subscription) };

		set_sub_status(subscription, subscription_status::UNSUBSCRIBED);

		_connection->send_message(message);
	}
}