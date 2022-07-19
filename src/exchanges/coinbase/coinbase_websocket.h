#pragma once

#include "common/json/json.h"
#include "exchanges/websockets/exchange_websocket_stream.h"
#include "exchanges/websockets/ohlcv_subscription_service.h"
#include "exchanges/exchange.h"
#include "trading/ohlcv_from_trades.h"

namespace mb::internal
{
	class coinbase_websocket_stream : public exchange_websocket_stream
	{
	private:
		concurrent_wrapper<ohlcv_subscription_service> _ohlcvSubscriptionService;

		void process_trade_message(const json_document& json);

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