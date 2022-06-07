#include "bybit_websocket.h"
#include "logging/logger.h"
#include "common/utils/containerutils.h"
#include "common/json/json.h"
#include "exchanges/exchange_ids.h"

#include "common/exceptions/not_implemented_exception.h"

namespace
{
	using namespace mb;

	std::string get_kline_topic_name(ohlcv_interval interval)
	{
		switch (interval)
		{
		case ohlcv_interval::M1:
			return "kline_1m";
		case ohlcv_interval::M5:
			return "kline_5m";
		case ohlcv_interval::M15:
			return "kline_15m";
		case ohlcv_interval::H1:
			return "kline_1h";
		case ohlcv_interval::D1:
			return "kline_1d";
		case ohlcv_interval::W1:
			return "kline_1w";
		default:
			throw mb_exception{ std::format("OHLCV interval not supported on ByBit") };
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
			.add("params", paramsJson.to_json())
			.to_string();
	}

	std::string create_message(const websocket_subscription& subscription, std::string_view eventName)
	{
		switch (subscription.channel())
		{
		case websocket_channel::PRICE:
			return create_message("trade", eventName, subscription.pairs());
		case websocket_channel::OHLCV:
			return create_message(get_kline_topic_name(subscription.get_ohlcv_interval()), eventName, subscription.pairs());
		case websocket_channel::ORDER_BOOK:
		default:
			throw mb_exception{ std::format("Websocket channel not supported on ByBit") };
		}
	}

	void process_price_message(std::unordered_map<std::string, double>& prices, std::string symbol, json_element data)
	{
		prices[symbol] = std::stod(data.begin().value().get<std::string>("p"));
	}

	void process_kline_message(std::unordered_map<std::string, ohlcv_data>& ohlcvData, std::string symbol, json_element data)
	{
		json_element klineElement = data.begin().value();

		ohlcvData.insert_or_assign(symbol, ohlcv_data
		{
			std::stod(klineElement.get<std::string>("o")),
			std::stod(klineElement.get<std::string>("h")),
			std::stod(klineElement.get<std::string>("l")),
			std::stod(klineElement.get<std::string>("c")),
			std::stod(klineElement.get<std::string>("v"))
		});
	}
}

namespace mb::internal
{
	bybit_websocket_stream::bybit_websocket_stream(std::unique_ptr<websocket_connection> connection)
		: exchange_websocket_stream{ exchange_ids::BYBIT, std::move(connection) }
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

		std::string topic{ json.get<std::string>("topic") };
		std::string symbol{ json.get<std::string>("symbol") };
		json_element dataElement{ json.element("data") };

		if (topic == "trade")
		{
			process_price_message(_prices, std::move(symbol), std::move(dataElement));
		}
		else if (topic.contains("kline"))
		{
			process_kline_message(_ohlcvData, std::move(symbol), std::move(dataElement));
		}
		else
		{
			logger::instance().warning("ByBit websocket unknown message: {}", message.data());
		}
	}

	void bybit_websocket_stream::subscribe(const websocket_subscription& subscription)
	{
		std::string message{ create_message(subscription, "sub") };
		_connection->send_message(message);
	}

	void bybit_websocket_stream::unsubscribe(const websocket_subscription& subscription)
	{
		std::string message{ create_message(subscription, "cancel") };
		_connection->send_message(message);
	}

	bool bybit_websocket_stream::is_subscribed(const websocket_subscription& subscription)
	{
		return true;
	}
}