#pragma once

#include "common/json/json.h"
#include "exchanges/websockets/exchange_websocket_stream.h"
#include "exchanges/exchange.h"
#include "trading/ohlcv_from_trades.h"

namespace mb::internal
{
	class coinbase_websocket_stream : public exchange_websocket_stream
	{
	private:
		static constexpr std::string_view URL = "wss://ws-feed.exchange.coinbase.com";

		concurrent_wrapper<unordered_string_map<std::unordered_map<int, ohlcv_from_trades>>> _ohlcvSubscriptions;
		std::unique_ptr<market_api> _marketApi;

		void process_price_message(const json_document& json);
		void ohlcv_virtual_subscribe(const websocket_subscription& subscription);
		void ohlcv_virtual_unsubscribe(const websocket_subscription& subscription);

		void on_message(std::string_view message) override;
		std::string generate_subscription_id(const unique_websocket_subscription& subscription) const override;

	public:
		coinbase_websocket_stream(
			std::unique_ptr<websocket_connection_factory> connectionFactory,
			std::unique_ptr<market_api> marketApi);

		void subscribe(const websocket_subscription& subscription) override;
		void unsubscribe(const websocket_subscription& subscription) override;
	};
}