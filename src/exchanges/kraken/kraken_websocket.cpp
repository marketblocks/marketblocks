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
		{ websocket_channel::TRADE, "trade" },
		{ websocket_channel::OHLCV, "ohlc" },
		{ websocket_channel::ORDER_BOOK, "book" }
	};

	static constexpr const char PAIR_SEPARATOR = '/';

	std::string create_message(std::string eventName, const websocket_subscription& subscription)
	{
		std::vector<std::string> tradablePairList{ to_vector<std::string>(subscription.pair_item(), 
			[](const tradable_pair& pair) { return pair.to_string(PAIR_SEPARATOR); })};

		json_writer subscriptionJson;
		subscriptionJson.add("name", CHANNEL_NAMES[subscription.channel()]);

		if (subscription.channel() == websocket_channel::OHLCV)
		{
			subscriptionJson.add("interval", to_minutes(subscription.get_ohlcv_interval()));
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
		: 
		exchange_websocket_stream
		{ 
			exchange_ids::KRAKEN,
			"wss://ws.kraken.com",
			'/',
			std::move(connectionFactory) 
		}
	{}

	void kraken_websocket_stream::process_event_message(const json_document& json)
	{
		// TODO
	}

	void kraken_websocket_stream::process_trade_message(std::string pairName, const json_document& json)
	{
		json_element tradesArray{ json.element(1) };

		for (auto it = tradesArray.begin(); it != tradesArray.end(); ++it)
		{
			json_element tradeElement{ it.value() };

			double price{ std::stod(tradeElement.get<std::string>(0)) };
			double volume{ std::stod(tradeElement.get<std::string>(1)) };
			std::time_t time{ std::stoll(tradeElement.get<std::string>(2)) };

			update_trade(pairName, trade_update{ time, price, volume });
		}
	}

	void kraken_websocket_stream::process_ohlcv_message(std::string pairName, std::string channelName, const json_document& json)
	{
		json_element ohlcArray{ json.element(1) };
		std::string minuteInterval{ split(channelName, '-')[1] };
		ohlcv_interval interval{ from_minutes(std::stoi(minuteInterval)) };

		update_ohlcv(std::move(pairName), interval, ohlcv_data
			{
				std::stoll(ohlcArray.get<std::string>(0)),
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

			if (channelName == "trade")
			{
				process_trade_message(std::move(pairName), json);
			}
			else if (channelName.contains("ohlc"))
			{
				process_ohlcv_message(std::move(pairName), std::move(channelName), json);
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

	void kraken_websocket_stream::send_subscribe(const websocket_subscription& subscription)
	{
		std::string message{ create_message("subscribe", subscription) };

		_connection->send_message(message);
	}

	void kraken_websocket_stream::send_unsubscribe(const websocket_subscription& subscription)
	{
		std::string message{ create_message("unsubscribe", subscription) };

		_connection->send_message(message);
	}
}