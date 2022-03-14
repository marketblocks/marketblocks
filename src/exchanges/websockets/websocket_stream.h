#pragma once

#include "local_order_book.h"
#include "networking/websocket/websocket_connection.h"
#include "exchanges/exchange_status.h"
#include "trading/tradable_pair.h"

namespace cb
{
	class websocket_stream
	{
	private:
		std::shared_ptr<websocket_client> _websocketClient;
		std::unique_ptr<websocket_connection> _connection;
		exchange_status _currentStatus;

	protected:
		local_order_book _localOrderBook;

		void send_message(std::string_view message) const;
		void log_status_change(exchange_status newStatus);

		virtual std::string stream_url() const = 0;
		virtual void on_open() = 0;
		virtual void on_close(std::string_view reason) = 0;
		virtual void on_fail(std::string_view reason) = 0;
		virtual void on_message(std::string_view message) = 0;

	public:
		websocket_stream(std::shared_ptr<websocket_client> websocketClient);

		void connect();
		ws_connection_status connection_status() const;
		exchange_status get_exchange_status() const noexcept { return _currentStatus; }

		void set_order_book_message_handler(std::function<void(tradable_pair)> onMessage) noexcept { _localOrderBook.set_message_handler(onMessage); }
		order_book_state get_order_book(const tradable_pair& pair) const { _localOrderBook.get_order_book(pair); }

		virtual void subscribe_order_book(const std::vector<tradable_pair>& tradablePairs) = 0;
	};
}