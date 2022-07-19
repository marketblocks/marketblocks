#pragma once

#include "common/json/json.h"
#include "exchanges/websockets/exchange_websocket_stream.h"

namespace mb::internal
{
	class bybit_websocket_stream : public exchange_websocket_stream
	{
	private:
		void process_trade_message(std::string subscriptionId, const json_document& json);
		void process_ohlcv_message(std::string subscriptionId, const json_document& json);

		void on_message(std::string_view message) override;
		std::string generate_subscription_id(const unique_websocket_subscription& subscription) const override;

	public:
		bybit_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory);

		void subscribe(const websocket_subscription& subscription) override;
		void unsubscribe(const websocket_subscription& subscription) override;
	};
}