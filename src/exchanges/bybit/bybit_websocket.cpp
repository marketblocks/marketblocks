#include "bybit_websocket.h"
#include "logging/logger.h"
#include "common/utils/containerutils.h"
#include "common/utils/stringutils.h"
#include "exchanges/exchange_ids.h"

#include "common/exceptions/not_implemented_exception.h"

namespace
{
	using namespace mb;

	std::string get_kline_topic(ohlcv_interval interval)
	{
		return "kline_" + to_string(interval);
	}

	std::string get_topic(const websocket_subscription& subscription)
	{
		switch (subscription.channel())
		{
		case websocket_channel::TRADE:
			return "trade";
		case websocket_channel::OHLCV:
			return get_kline_topic(subscription.get_ohlcv_interval());
		case websocket_channel::ORDER_BOOK:
			return "diffDepth";
		default:
			throw mb_exception{ fmt::format("Websocket channel not supported on ByBit") };
		}
	}

	std::string create_message(std::string_view topic, std::string_view eventName, const std::vector<tradable_pair>& tradablePairs, std::unordered_map<std::string, std::string> params = {})
	{
		std::string tradablePairsList;
		for (auto& pair : tradablePairs)
		{
			tradablePairsList += pair.to_string();
			tradablePairsList += ",";
		}
		tradablePairsList.erase(std::prev(tradablePairsList.end()));

		json_writer paramsJson{};
		paramsJson.add("binary", false);

		for (auto& [key, value] : params)
		{
			paramsJson.add(key, value);
		}

		return json_writer{}
			.add("symbol", tradablePairsList)
			.add("topic", topic)
			.add("event", eventName)
			.add("params", paramsJson)
			.to_string();
	}

	order_book_entry create_order_book_entry(order_book_side side, const json_element& entryElement)
	{
		return order_book_entry
		{
			std::stod(entryElement.get<std::string>(0)),
			std::stod(entryElement.get<std::string>(1)),
			side
		};
	}

	order_book_cache create_order_book_cache(std::time_t timeStamp, int depth, const json_element& asksElement, const json_element& bidsElement)
	{
		ask_cache askCache;
		bid_cache bidCache;
		
		for (int i = 0; i < depth; ++i)
		{
			if (i < asksElement.size())
			{
				askCache.emplace(create_order_book_entry(order_book_side::ASK, asksElement.element(i)));
			}

			if (i < bidsElement.size())
			{
				bidCache.emplace(create_order_book_entry(order_book_side::BID, bidsElement.element(i)));
			}
		}

		return order_book_cache{ timeStamp, std::move(askCache), std::move(bidCache) };
	}
}

namespace mb::internal
{
	bybit_websocket_stream::bybit_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory)
		: 
		exchange_websocket_stream
		{ 
			exchange_ids::BYBIT, 
			"wss://stream.bybit.com/spot/quote/ws/v1",
			'\0',
			std::move(connectionFactory)
		}
	{}

	void bybit_websocket_stream::on_message(std::string_view message)
	{
		json_document json{ parse_json(message) };

		if (json.has_member("code"))
		{
			std::string errorCode{ json.get<std::string>("code") };

			if (errorCode != "0")
			{
				logger::instance().error("ByBit websocket error. Error code: {0}, Message: {1}",
					errorCode,
					json.get<std::string>("desc"));
			}
		}

		std::string symbol{ json.get<std::string>("symbol") };
		std::string topic{ json.get<std::string>("topic") };

		if (topic == "trade")
		{
			process_trade_message(std::move(symbol), json);
		}
		else if (topic == "kline")
		{
			process_ohlcv_message(std::move(symbol), json);
		}
		else if (topic == "diffDepth")
		{
			process_order_book_message(std::move(symbol), json);
		}
	}

	void bybit_websocket_stream::process_trade_message(std::string pairName, const json_document& json)
	{
		json_element dataElement{ json.element("data").begin().value() };
		
		double price{ std::stod(dataElement.get<std::string>("p")) };
		double volume{ std::stod(dataElement.get<std::string>("q")) };
		std::time_t time{ dataElement.get<std::time_t>("t") / 1000 };
		
		update_trade(std::move(pairName), trade_update{time, price, volume});
	}

	void bybit_websocket_stream::process_ohlcv_message(std::string pairName, const json_document& json)
	{
		json_element dataElement{ json.element("data").begin().value() };

		ohlcv_data data
		{
			dataElement.get<std::time_t>("t") / 1000,
			std::stod(dataElement.get<std::string>("o")),
			std::stod(dataElement.get<std::string>("h")),
			std::stod(dataElement.get<std::string>("l")),
			std::stod(dataElement.get<std::string>("c")),
			std::stod(dataElement.get<std::string>("v"))
		};

		ohlcv_interval interval{ parse_ohlcv_interval(json.element("params").get<std::string>("klineType")) };
		update_ohlcv(std::move(pairName), interval, std::move(data));
	}

	void bybit_websocket_stream::process_order_book_message(std::string pairName, const json_document& json)
	{
		json_element dataElement{ json.element("data").begin().value() };
		std::time_t timeStamp{ dataElement.get<std::time_t>("t") };

		json_element asksElement{ dataElement.element("a") };
		json_element bidsElement{ dataElement.element("b") };
		int depth = std::max<int>(asksElement.size(), bidsElement.size());

		if (json.get<bool>("f"))
		{
			order_book_cache orderBook{ create_order_book_cache(timeStamp, depth, asksElement, bidsElement) };
			initialise_order_book(std::move(pairName), std::move(orderBook));
			return;
		}

		for (int i = 0; i < depth; ++i)
		{
			if (i < asksElement.size())
			{
				update_order_book(pairName, timeStamp, create_order_book_entry(order_book_side::ASK, asksElement.element(i)));
			}

			if (i < bidsElement.size())
			{
				update_order_book(pairName, timeStamp, create_order_book_entry(order_book_side::BID, bidsElement.element(i)));
			}
		}
	}

	void bybit_websocket_stream::send_subscribe(const websocket_subscription& subscription)
	{
		std::string topic{ get_topic(subscription) };
		std::string message{ create_message(topic, "sub", subscription.pair_item()) };

		_connection->send_message(message);
	}

	void bybit_websocket_stream::send_unsubscribe(const websocket_subscription& subscription)
	{
		std::string topic{ get_topic(subscription) };
		std::string message{ create_message(topic, "cancel", subscription.pair_item()) };

		_connection->send_message(message);
	}
}