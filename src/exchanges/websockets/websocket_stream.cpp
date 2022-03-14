#include "websocket_stream.h"
#include "logging/logger.h"

namespace
{
	#define CB_ASSERT_CONNECTION_EXISTS(connection) (assert(connection, "Connection has not been created"))
}

namespace cb
{
	websocket_stream::websocket_stream(std::shared_ptr<websocket_client> websocketClient)
		: 
		_websocketClient{ websocketClient }, 
		_currentStatus{ exchange_status::OFFLINE },
		_connection{},
		_localOrderBook{}
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
				[this]() { on_open(); },
				[this](const std::string& reason) { on_close(reason); },
				[this](const std::string& reason) { on_fail(reason); },
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

	void websocket_stream::send_message(std::string_view message) const
	{
		CB_ASSERT_CONNECTION_EXISTS(_connection);

		_connection->send_message(message);
	}

	void websocket_stream::log_status_change(exchange_status newStatus)
	{
		_currentStatus = newStatus;
		logger::instance().warning("Websocket stream status changed. New status: {}", to_string(newStatus));
	}
}