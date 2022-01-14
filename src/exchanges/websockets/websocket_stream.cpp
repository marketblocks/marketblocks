#include "websocket_stream.h"

void WebsocketStream::connect(std::shared_ptr<WebsocketClient> websocketClient)
{
	WebsocketEventHandlers eventHandlers
	{
		[this](const std::string& message) { on_message(message); }
	};

	_connection = std::make_unique<WebsocketConnection>(
		create_websocket_connection(websocketClient, stream_url(), eventHandlers));
}

WsConnectionStatus WebsocketStream::connection_status() const
{
	if (_connection)
	{
		return _connection->connection_status();
	}

	return WsConnectionStatus::CLOSED;
}

void WebsocketStream::subscribe_order_book(const std::vector<TradablePair>& tradablePairs)
{
	std::string subscriptionMessage = get_subscribe_order_book_message(tradablePairs);

	for (auto& pair : tradablePairs)
	{
		_orderBookCaches.emplace(pair.iso_4217_a3(), OrderBookCache{ 10 });
	}

	_connection->send_message(subscriptionMessage);
}

OrderBookState WebsocketStream::get_order_book_snapshot(const TradablePair& tradablePair) const
{
	return _orderBookCaches.at(tradablePair.iso_4217_a3()).snapshot();
}