#pragma once

#include "order_book_cache.h"
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
		std::unordered_map<std::string, order_book_cache> _orderBookCaches;

		exchange_status _currentStatus;

	protected:
		void send_message(const std::string& message) const;
		void log_status_change(exchange_status newStatus);

		bool is_order_book_subscribed(const std::string& pair);
		void initialise_order_book_cache(const std::string& pair, std::vector<cache_entry> asks, std::vector<cache_entry> bids);
		void update_order_book_cache(const std::string& pair, cache_entry cacheEntry);
		void replace_in_order_book_cache(const std::string& pair, cache_replacement cacheReplacement);

		virtual std::string stream_url() const = 0;
		virtual void on_open() = 0;
		virtual void on_close(const std::string& reason) = 0;
		virtual void on_fail(const std::string& reason) = 0;
		virtual void on_message(const std::string& message) = 0;

	public:
		websocket_stream(std::shared_ptr<websocket_client> websocketClient);

		void connect();
		ws_connection_status connection_status() const;
		exchange_status get_exchange_status() const { return _currentStatus; }

		virtual void subscribe_order_book(const std::vector<tradable_pair>& tradablePairs) = 0;

		order_book_state get_order_book_snapshot(const tradable_pair& tradablePair) const;
	};
}