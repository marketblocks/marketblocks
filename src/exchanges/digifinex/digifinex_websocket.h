#pragma once

#include "common/json/json.h"
#include "exchanges/websockets/exchange_websocket_stream.h"
#include "exchanges/websockets/ohlcv_subscription_service.h"

namespace mb::internal
{
	class digifinex_websocket_stream : public exchange_websocket_stream
	{
	private:
		concurrent_wrapper<ohlcv_subscription_service> _ohlcvSubscriptionService;

		void process_trade_message(const json_document& json);

		void on_message(std::string_view message) override;
		void send_subscribe(const websocket_subscription& subscription) override;
		void send_unsubscribe(const websocket_subscription& subscription) override;

	public:
		digifinex_websocket_stream(
			std::unique_ptr<websocket_connection_factory> connectionFactory,
			std::unique_ptr<market_api> marketApi);
	};
}