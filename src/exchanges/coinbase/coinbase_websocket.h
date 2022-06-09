#pragma once

#include "exchanges/websockets/exchange_websocket_stream.h"

namespace mb::internal
{
	class coinbase_websocket_stream : public exchange_websocket_stream
	{
	private:
		static constexpr std::string_view URL = "wss://ws-feed.exchange.coinbase.com";

		void on_message(std::string_view message) override;

	public:
		coinbase_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory);

		void subscribe(const websocket_subscription& subscription) override;
		void unsubscribe(const websocket_subscription& subscription) override;
	};
}