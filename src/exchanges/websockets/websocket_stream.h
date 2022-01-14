#pragma once

#include "order_book_cache.h"
#include "networking/websocket/websocket_connection.h"
#include "trading/tradable_pair.h"

namespace cb
{
	class websocket_stream
	{
	private:
		std::unique_ptr<websocket_connection> _connection;

	protected:
		std::unordered_map<std::string, order_book_cache> _orderBookCaches;

		virtual std::string stream_url() const = 0;
		virtual std::string get_subscribe_order_book_message(const std::vector<tradable_pair>& tradablePairs) const = 0;

		virtual void on_message(const std::string& message) = 0;

	public:
		void connect(std::shared_ptr<websocket_client> websocketClient);

		ws_connection_status connection_status() const;

		void subscribe_order_book(const std::vector<tradable_pair>& tradablePairs);
		order_book_state get_order_book_snapshot(const tradable_pair& tradablePair) const;
	};
}