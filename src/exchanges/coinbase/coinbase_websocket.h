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
		void process_order_book_initialisation(const json_document& json);
		void process_order_book_update(const json_document& json);

		void on_message(std::string_view message) override;
		void send_subscribe(const websocket_subscription& subscription) override;
		void send_unsubscribe(const websocket_subscription& subscription) override;

	public:
		coinbase_websocket_stream(
			std::unique_ptr<websocket_connection_factory> connectionFactory,
			std::unique_ptr<market_api> marketApi);
	};
}