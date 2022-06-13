#pragma once

#include "common/json/json.h"
#include "exchanges/websockets/exchange_websocket_stream.h"

namespace mb::internal
{
	class coinbase_websocket_stream : public exchange_websocket_stream
	{
	private:
		static constexpr std::string_view URL = "wss://ws-feed.exchange.coinbase.com";

		void set_sub_status(std::string channel, const websocket_subscription& subscription, subscription_status status);
		void process_price_message(const json_document& json);

		void on_message(std::string_view message) override;
		std::string generate_subscription_id(const unique_websocket_subscription& subscription) const override;

	public:
		coinbase_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory);

		void subscribe(const websocket_subscription& subscription) override;
		void unsubscribe(const websocket_subscription& subscription) override;
	};
}