#include "exchange.h"

namespace mb
{
	exchange::exchange(std::string_view id, std::shared_ptr<websocket_stream> websocketStream)
		: _id{ std::move(id) }, _websocketStream{ std::move(websocketStream) }, _websocketConnected{ false }
	{}

	std::shared_ptr<websocket_stream> exchange::get_websocket_stream()
	{
		if (!_websocketConnected)
		{
			if (_websocketStream->connection_status() == ws_connection_status::CLOSED)
			{
				_websocketStream->reset();
			}

			_websocketConnected = true;
		}

		return _websocketStream;
	}
}