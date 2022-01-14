#include "websocket_stream.h"

namespace cb
{
	void websocket_stream::connect(std::shared_ptr<websocket_client> websocketClient)
	{
		websocket_event_handlers eventHandlers
		{
			[this](const std::string& message) { on_message(message); }
		};

		_connection = std::make_unique<websocket_connection>(
			create_websocket_connection(websocketClient, stream_url(), eventHandlers));
	}

	ws_connection_status websocket_stream::connection_status() const
	{
		if (_connection)
		{
			return _connection->connection_status();
		}

		return ws_connection_status::CLOSED;
	}

	void websocket_stream::subscribe_order_book(const std::vector<tradable_pair>& tradablePairs)
	{
		std::string subscriptionMessage = get_subscribe_order_book_message(tradablePairs);

		for (auto& pair : tradablePairs)
		{
			_orderBookCaches.emplace(pair.iso_4217_a3(), order_book_cache{ 10 });
		}

		_connection->send_message(subscriptionMessage);
	}

	order_book_state websocket_stream::get_order_book_snapshot(const tradable_pair& tradablePair) const
	{
		return _orderBookCaches.at(tradablePair.iso_4217_a3()).snapshot();
	}
}