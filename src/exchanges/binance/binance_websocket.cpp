#include "binance_websocket.h"
#include "exchanges/exchange_ids.h"
#include "common/utils/containerutils.h"
#include "logging/logger.h"

namespace
{
	using namespace mb;

	std::string create_message(std::string method, std::string channel, const std::vector<tradable_pair>& pairs)
	{
		std::vector<std::string> params{ to_vector<std::string>(pairs, [&channel](const tradable_pair& pair)
			{
				std::string pairName{ pair.to_string() };
				to_lower(pairName);
				return std::move(pairName) + "@" + channel;
			}) };

		json_writer json;
		json.add("method", std::move(method));
		json.add("params", std::move(params));
		json.add("id", 1);

		return json.to_string();
	}

	std::string get_kline_channel_name(std::string interval)
	{
		return "kline_" + std::move(interval);
	}

	template<typename Subscription>
	std::string get_channel_name(const Subscription& subscription)
	{
		switch (subscription.channel())
		{
		case websocket_channel::PRICE:
			return "trade";
		case websocket_channel::OHLCV:
			return get_kline_channel_name(to_string(subscription.get_ohlcv_interval()));
		case websocket_channel::ORDER_BOOK:
		default:
			throw mb_exception{ "Websocket channel not supported on Binance" };
		}
	}

	std::string generate_subscription_id(std::string symbol, std::string channel)
	{
		return std::move(symbol) + std::move(channel);
	}
}

namespace mb::internal
{
	binance_websocket_stream::binance_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory)
		: exchange_websocket_stream{ exchange_ids::BINANCE, "wss://stream.binance.com:9443/ws", std::move(connectionFactory) }
	{}

	std::string binance_websocket_stream::generate_subscription_id(const unique_websocket_subscription& subscription) const
	{
		std::string pair{ subscription.pair_item().to_string() };
		std::string channel{ get_channel_name(subscription) };

		return ::generate_subscription_id(std::move(pair), std::move(channel));
	}

	void binance_websocket_stream::process_price_message(const json_document& json)
	{
		std::string symbol{ json.get<std::string>("s") };
		double price{ std::stod(json.get<std::string>("p")) };
		std::string subId{ ::generate_subscription_id(std::move(symbol), "trade") };

		update_price(std::move(subId), price);
	}

	void binance_websocket_stream::process_ohlcv_message(const json_document& json)
	{
		std::string symbol{ json.get<std::string>("s") };
		json_element klineElement{ json.element("k") };
		std::string interval{ klineElement.get<std::string>("i") };
		std::string subId{ ::generate_subscription_id(std::move(symbol), get_kline_channel_name(std::move(interval))) };

		ohlcv_data ohlcv
		{
			klineElement.get<std::time_t>("t") / 1000,
			std::stod(klineElement.get<std::string>("o")),
			std::stod(klineElement.get<std::string>("h")),
			std::stod(klineElement.get<std::string>("l")),
			std::stod(klineElement.get<std::string>("c")),
			std::stod(klineElement.get<std::string>("v"))
		};

		update_ohlcv(std::move(subId), std::move(ohlcv));
	}

	void binance_websocket_stream::on_message(std::string_view message)
	{
		json_document json{ parse_json(message) };

		if (json.has_member("msg"))
		{
			logger::instance().error("Binance websocket: {}", json.get<std::string>("msg"));
			return;
		}

		if (json.has_member("e"))
		{
			std::string channel{ json.get<std::string>("e") };

			if (channel == "trade")
			{
				process_price_message(json);
				return;
			}
			else if (channel == "kline")
			{
				process_ohlcv_message(json);
				return;
			}
		}

		logger::instance().info("Binance message: {}", message);
	}

	void binance_websocket_stream::subscribe(const websocket_subscription& subscription)
	{
		std::string message{ create_message("SUBSCRIBE", get_channel_name(subscription), subscription.pair_item()) };
		_connection->send_message(std::move(message));
	}

	void binance_websocket_stream::unsubscribe(const websocket_subscription& subscription)
	{
		std::string message{ create_message("UNSUBSCRIBE", get_channel_name(subscription), subscription.pair_item()) };
		_connection->send_message(std::move(message));
	}
}