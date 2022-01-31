#include "websocket_stream.h"
#include "logging/logger.h"

namespace
{
	#define CB_ASSERT_CONNECTION_EXISTS(connection) (assert(connection, "Connection has not been created"))
}

namespace cb
{
	websocket_stream::websocket_stream(std::shared_ptr<websocket_client> websocketClient)
		: _websocketClient{ websocketClient }, _currentStatus{ exchange_status::OFFLINE }
	{}

	void websocket_stream::connect()
	{
		if (connection_status() == ws_connection_status::OPEN)
		{
			return;
		}

		_connection = std::make_unique<websocket_connection>(
			create_websocket_connection(
				_websocketClient, 
				stream_url(), 
				[this](const std::string& message) { on_message(message); }));
	}

	ws_connection_status websocket_stream::connection_status() const
	{
		if (_connection)
		{
			return _connection->connection_status();
		}

		return ws_connection_status::CLOSED;
	}

	void websocket_stream::send_message(const std::string& message) const
	{
		CB_ASSERT_CONNECTION_EXISTS(_connection);

		_connection->send_message(message);
	}

	void websocket_stream::log_status_change(exchange_status newStatus)
	{
		_currentStatus = newStatus;
		logger::instance().warning("Websocket stream status changed. New status: {}", to_string(newStatus));
	}

	bool websocket_stream::is_order_book_subscribed(const std::string& pair)
	{
		return _orderBookCaches.contains(pair);
	}

	void websocket_stream::initialise_order_book_cache(const std::string& pair, std::vector<cache_entry> asks, std::vector<cache_entry> bids)
	{
		_orderBookCaches.emplace(pair, order_book_cache{ std::move(asks), std::move(bids) });
	}

	void websocket_stream::update_order_book_cache(const std::string& pair, cache_entry cacheEntry)
	{
		auto cacheIterator = _orderBookCaches.find(pair);
		if (cacheIterator != _orderBookCaches.end())
		{
			cacheIterator->second.cache(std::move(cacheEntry));
		}
	}

	void websocket_stream::replace_in_order_book_cache(const std::string& pair, cache_replacement cacheReplacement)
	{
		auto cacheIterator = _orderBookCaches.find(pair);
		if (cacheIterator != _orderBookCaches.end())
		{
			cacheIterator->second.replace(std::move(cacheReplacement));
		}
	}

	order_book_state websocket_stream::get_order_book_snapshot(const tradable_pair& tradablePair) const
	{
		CB_ASSERT_CONNECTION_EXISTS(_connection);

		auto cacheIterator = _orderBookCaches.find(tradablePair.iso_4217_a3());

		if (cacheIterator != _orderBookCaches.end())
		{
			return cacheIterator->second.snapshot();
		}

		throw cb_exception{ "Order book for pair '" + tradablePair.iso_4217_a3() + "' is not subscribed" };
	}
}