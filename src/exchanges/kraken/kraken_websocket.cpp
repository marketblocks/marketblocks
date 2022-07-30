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
		else if (subscription.channel() == websocket_channel::ORDER_BOOK)
		{
			subscriptionJson.add("depth", 100);
		}
		
		return json_writer{}
			.add("event", eventName)
			.add("pair", std::move(tradablePairList))
			.add("subscription", subscriptionJson)
			.to_string();
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

	std::time_t get_order_book_update_timestamp(const json_element& entryElement)
	{
		return std::time_t{ std::stoll(entryElement.get<std::string>(2)) };
	}

	order_book_cache create_order_book_cache(const json_element& json)
	{
		json_element asks = json.element("as");
		json_element bids = json.element("bs");

		int depth = std::max<int>(asks.size(), bids.size());

		ask_cache askCache;
		bid_cache bidCache;
		std::time_t timeStamp = 0;

		for (int i = 0; i < depth; ++i)
		{
			if (i < asks.size())
			{
				json_element askElement{ asks.element(i) };
				askCache.emplace(create_order_book_entry(order_book_side::ASK, askElement));

				timeStamp = std::max(timeStamp, get_order_book_update_timestamp(askElement));
			}

			if (i < bids.size())
			{
				json_element bidElement{ bids.element(i) };
				bidCache.emplace(create_order_book_entry(order_book_side::BID, bidElement));

				timeStamp = std::max(timeStamp, get_order_book_update_timestamp(bidElement));
			}
		}

		return order_book_cache{ timeStamp, std::move(askCache), std::move(bidCache) };
	}
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

	void kraken_websocket_stream::process_order_book_message(std::string pairName, const json_document& json)
	{
		if (json.size() == 4)
		{
			json_element entryObject = json.element(1);

			if (entryObject.has_member("as"))
			{
				order_book_cache cache{ create_order_book_cache(entryObject) };
				initialise_order_book(std::move(pairName), std::move(cache));
			}
			else
			{
				process_order_book_updates(std::move(pairName), entryObject);
			}
		}
		else
		{
			process_order_book_updates(pairName, json.element(1));
			process_order_book_updates(std::move(pairName), json.element(2));
		}
	}

	void kraken_websocket_stream::process_order_book_updates(std::string pairName, const json_element& updateObject)
	{
		order_book_side side;
		std::string name;

		if (updateObject.has_member("a"))
		{
			side = order_book_side::ASK;
			name = "a";
		}
		else
		{
			side = order_book_side::BID;
			name = "b";
		}

		json_element updateElement{ updateObject.element(name) };

		for (auto it = updateElement.begin(); it != updateElement.end(); ++it)
		{
			json_element entryElement{ it.value() };
			order_book_entry entry{ create_order_book_entry(side, entryElement) };
			std::time_t timeStamp{ get_order_book_update_timestamp(entryElement) };

			update_order_book(pairName, timeStamp, std::move(entry));
		}
	}

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
			else if (contains(channelName, "ohlc"))
			{
				process_ohlcv_message(std::move(pairName), std::move(channelName), json);
			}
			else if (contains(channelName, "book"))
			{
				process_order_book_message(std::move(pairName), json);
			}
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