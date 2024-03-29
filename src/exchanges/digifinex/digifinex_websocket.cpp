#include "digifinex_websocket.h"
#include "exchanges/exchange_ids.h"
#include "common/utils/containerutils.h"

namespace
{
	using namespace mb;

	constexpr const char PAIR_SEPARATOR = '_';

	std::string get_channel(websocket_channel channel)
	{
		switch (channel)
		{
		case websocket_channel::TRADE:
			return "trades";
		case websocket_channel::ORDER_BOOK:
		default:
			throw mb_exception{ fmt::format("Websocket channel not supported on Digfinex") };
		}
	}

	std::string create_message(std::string action, std::string channel, const std::vector<tradable_pair>& pairs)
	{
		std::vector<std::string> tradablePairList{ to_vector<std::string>(pairs, [](const tradable_pair& pair)
		{
			return pair.to_string(PAIR_SEPARATOR);
		}) };

		return json_writer{}
			.add("method", channel + "." + action)
			.add("params", std::move(tradablePairList))
			.to_string();
	}
}

namespace mb::internal
{
	digifinex_websocket_stream::digifinex_websocket_stream(
		std::unique_ptr<websocket_connection_factory> connectionFactory,
		std::unique_ptr<market_api> marketApi)
		: 
		exchange_websocket_stream
		{
			exchange_ids::DIGIFINEX, 
			"wss://openapi.digifinex.com/ws/v1/",
			PAIR_SEPARATOR,
			std::move(connectionFactory)
		},
		_ohlcvSubscriptionService{ ohlcv_subscription_service
			{
				std::move(marketApi),
				[this](std::string pairName, ohlcv_interval interval, ohlcv_data ohlcv) { update_ohlcv(std::move(pairName), interval, std::move(ohlcv)); },
				PAIR_SEPARATOR
			} }
	{}

	void digifinex_websocket_stream::process_trade_message(const json_document& json)
	{
		json_element paramsElement{ json.element("params") };
		json_element tradesElement{ paramsElement.element(1) };
		std::string pairName{ paramsElement.get<std::string>(2) };

		json_element lastTrade{ tradesElement.element(tradesElement.size() - 1) };

		double price{ std::stod(lastTrade.get<std::string>("price")) };
		double volume{ std::stod(lastTrade.get<std::string>("amount")) };
		std::time_t time{ lastTrade.get<std::time_t>("time") };

		update_trade(pairName, trade_update{time, price, volume});

		auto lockedOhlcvSubscriptions = _ohlcvSubscriptionService.unique_lock();

		if (lockedOhlcvSubscriptions->is_subscribed(pairName))
		{
			lockedOhlcvSubscriptions->update_ohlcv(pairName, time, price, volume);
		}
	}

	void digifinex_websocket_stream::on_message(std::string_view message)
	{
		json_document json{ parse_json(message) };

		std::string method{ json.get<std::string>("method") };

		if (method == "trades.update")
		{
			process_trade_message(json);
		}
	}

	void digifinex_websocket_stream::send_subscribe(const websocket_subscription& subscription)
	{
		if (subscription.channel() == websocket_channel::OHLCV)
		{
			send_subscribe(websocket_subscription::create_trade_sub(subscription.pair_item()));
			_ohlcvSubscriptionService.unique_lock()->add_subscription(subscription);
			return;
		}

		std::string channelName{ get_channel(subscription.channel()) };
		std::string message{ create_message("subscribe", std::move(channelName), subscription.pair_item()) };

		_connection->send_message(std::move(message));
	}

	void digifinex_websocket_stream::send_unsubscribe(const websocket_subscription& subscription)
	{
		if (subscription.channel() == websocket_channel::OHLCV)
		{
			_ohlcvSubscriptionService.unique_lock()->remove_subscription(subscription);
			return;
		}

		std::string channelName{ get_channel(subscription.channel()) };
		std::string message{ create_message("unsubscribe", std::move(channelName), subscription.pair_item()) };

		_connection->send_message(std::move(message));
	}
}