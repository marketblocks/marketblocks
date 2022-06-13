#pragma once

#include "exchanges/websockets/exchange_websocket_stream.h"
#include "common/json/json.h"

namespace mb::internal
{
	class kraken_websocket_stream : public exchange_websocket_stream
	{
	private:
		static constexpr std::string_view URL = "wss://ws.kraken.com";

		void set_sub_status(const websocket_subscription& subscription, subscription_status status);
		void process_event_message(const json_document& json);
		void process_price_message(std::string subscriptionId, const json_document& json);
		void process_ohlcv_message(std::string subscriptionId, const json_document& json);
		//void process_order_book_message(std::string subscriptionId, const json_document& json);

		void on_message(std::string_view message) override;
		std::string generate_subscription_id(const unique_websocket_subscription& subscription) const override;

	public:
		kraken_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory);

		void subscribe(const websocket_subscription& subscription) override;
		void unsubscribe(const websocket_subscription& subscription) override;
	};
}