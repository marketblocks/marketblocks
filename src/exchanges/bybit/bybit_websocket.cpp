#include "bybit_websocket.h"
#include "logging/logger.h"
#include "common/utils/containerutils.h"
#include "common/json/json.h"

#include "common/exceptions/not_implemented_exception.h"

namespace
{
	using namespace mb;

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

	std::string get_kline_topic_name(int interval)
	{
		return "kline_1m";
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
	void bybit_websocket_stream::on_open()
	{
		logger::instance().info("Bybit websocket connection opened");
	}

	void bybit_websocket_stream::on_close(std::error_code reason)
	{
		logger::instance().error("Connection has been closed: {}", reason.message());
	}

	void bybit_websocket_stream::on_fail(std::error_code reason)
	{
		logger::instance().info("Connection failed: {}", reason.message());
	}

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

	std::string bybit_websocket_stream::get_order_book_subscription_message(const std::vector<tradable_pair>& tradablePairs) const
	{
		throw not_implemented_exception{ "bybit_websocket::order_book" };
	}

	std::string bybit_websocket_stream::get_order_book_unsubscription_message(const std::vector<tradable_pair>& tradablePairs) const
	{
		throw not_implemented_exception{ "bybit_websocket::order_book" };
	}

	std::string bybit_websocket_stream::get_price_subscription_message(const std::vector<tradable_pair>& tradablePairs) const
	{
		return create_message("trade", "sub", tradablePairs);
	}

	std::string bybit_websocket_stream::get_price_unsubscription_message(const std::vector<tradable_pair>& tradablePairs) const
	{
		return create_message("trade", "cancel", tradablePairs);
	}

	bool bybit_websocket_stream::is_price_subscribed(const tradable_pair& pair) const
	{
		return true;
	}

	double bybit_websocket_stream::get_price(const tradable_pair& pair) const
	{
		return find_or_default(_prices, pair.to_string(), 0.0);
	}

	std::string bybit_websocket_stream::get_candles_subscription_message(const std::vector<tradable_pair>& tradablePairs, int interval) const
	{
		std::string topic{ get_kline_topic_name(interval) };
		return create_message(topic, "sub", tradablePairs);
	}

	std::string bybit_websocket_stream::get_candles_unsubscription_message(const std::vector<tradable_pair>& tradablePairs, int interval) const
	{
		std::string topic{ get_kline_topic_name(interval) };
		return create_message(topic, "cancel", tradablePairs);
	}

	bool bybit_websocket_stream::is_candles_subscribed(const tradable_pair& pair, int interval) const
	{
		return true;
	}

	ohlcv_data bybit_websocket_stream::get_candle(const tradable_pair& pair, int interval) const
	{
		return find_or_default(_ohlcvData, pair.to_string(), ohlcv_data{0,0,0,0,0});
	}
}