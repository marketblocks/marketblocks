#include "websocket_stream.h"

void WebsocketStream::connect(std::shared_ptr<WebsocketClient> websocketClient)
{
	WebsocketEventHandlers eventHandlers
	{
		[this](const std::string& message) { onMessage(message); }
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