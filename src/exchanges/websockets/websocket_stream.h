#pragma once

#include "websocket_stream_implementation.h"
#include "networking/websocket/websocket_connection.h"
#include "exchanges/exchange_status.h"
#include "trading/tradable_pair.h"

namespace cb
{
	class websocket_stream
	{
	private:
		std::unique_ptr<internal::websocket_stream_implementation> _implementation;
		std::shared_ptr<websocket_client> _websocketClient;
		std::unique_ptr<websocket_connection> _connection;

		void send_message(std::string_view message) const;

	public:
		websocket_stream(
			std::unique_ptr<internal::websocket_stream_implementation> implementation,
			std::shared_ptr<websocket_client> websocketClient);

		void connect();
		ws_connection_status connection_status() const;
		exchange_status get_exchange_status() const noexcept { return _implementation->get_exchange_status(); }

		void set_order_book_message_handler(std::function<void(tradable_pair)> onMessage) noexcept { _implementation->set_order_book_message_handler(onMessage); }
		order_book_state get_order_book(const tradable_pair& pair, int depth = 0) const { return _implementation->get_order_book(pair, depth); }
		void subscribe_order_book(const std::vector<tradable_pair>& tradablePairs);
	};
}