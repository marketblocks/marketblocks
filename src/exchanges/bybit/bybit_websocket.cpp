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

	template<typename Subscription>
	std::string get_topic(const Subscription& subscription)
	{
		switch (subscription.channel())
		{
		case websocket_channel::PRICE:
			return "trade";
		case websocket_channel::OHLCV:
			return get_kline_topic(subscription.get_ohlcv_interval());
		case websocket_channel::ORDER_BOOK:
		default:
			throw mb_exception{ std::format("Websocket channel not supported on ByBit") };
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

	std::string generate_subscription_id(std::string symbol, std::string topic)
	{
		return std::move(symbol) + std::move(topic);
	}
}

namespace mb::internal
{
	bybit_websocket_stream::bybit_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory)
		: exchange_websocket_stream{ exchange_ids::BYBIT, URL, std::move(connectionFactory) }
	{}

	std::string bybit_websocket_stream::generate_subscription_id(const unique_websocket_subscription& subscription) const
	{
		std::string symbol{ subscription.pair_item().to_string() };
		std::string topic{ get_topic(subscription) };
		return ::generate_subscription_id(std::move(symbol), std::move(topic));
	}

	void bybit_websocket_stream::set_sub_status(std::string_view topic, const websocket_subscription& subscription, subscription_status status)
	{
		for (auto& pair : subscription.pair_item())
		{
			std::string subId{ ::generate_subscription_id(pair.to_string(), topic.data()) };
			update_subscription_status(subId, subscription.channel(), status);
		}
	}

	void bybit_websocket_stream::set_subscribed_if_first(std::string_view subscriptionId, websocket_channel channel, const json_document& json)
	{
		bool isFirst{ json.get<bool>("f") };
		if (isFirst)
		{
			update_subscription_status(subscriptionId.data(), channel, subscription_status::SUBSCRIBED);
		}
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

		std::string symbol{ json.get<std::string>("symbol") };
		std::string topic{ json.get<std::string>("topic") };
		std::string subscriptionId{ ::generate_subscription_id(symbol, topic) };

		if (topic == "trade")
		{
			process_price_message(std::move(subscriptionId), json);
		}
		else if (topic.contains("kline"))
		{
			process_ohlcv_message(std::move(subscriptionId), json);
		}
	}

	void bybit_websocket_stream::process_price_message(std::string subscriptionId, const json_document& json)
	{
		json_element dataElement{ json.element("data").begin().value() };
		
		double price{ std::stod(dataElement.get<std::string>("p")) };
		
		set_subscribed_if_first(subscriptionId, websocket_channel::PRICE, json);
		update_price(std::move(subscriptionId), price);
	}

	void bybit_websocket_stream::process_ohlcv_message(std::string subscriptionId, const json_document& json)
	{
		json_element dataElement{ json.element("data").begin().value() };

		ohlcv_data data
		{
			std::stod(dataElement.get<std::string>("o")),
			std::stod(dataElement.get<std::string>("h")),
			std::stod(dataElement.get<std::string>("l")),
			std::stod(dataElement.get<std::string>("c")),
			std::stod(dataElement.get<std::string>("v"))
		};

		set_subscribed_if_first(subscriptionId, websocket_channel::OHLCV, json);
		update_ohlcv(std::move(subscriptionId), std::move(data));
	}

	void bybit_websocket_stream::subscribe(const websocket_subscription& subscription)
	{
		std::string topic{ get_topic(subscription) };
		std::string message{ create_message(topic, "sub", subscription.pair_item()) };

		set_sub_status(topic, subscription, subscription_status::INITIALISING);

		_connection->send_message(message);
	}

	void bybit_websocket_stream::unsubscribe(const websocket_subscription& subscription)
	{
		std::string topic{ get_topic(subscription) };
		std::string message{ create_message(topic, "cancel", subscription.pair_item()) };

		set_sub_status(topic, subscription, subscription_status::UNSUBSCRIBED);

		_connection->send_message(message);
	}
}