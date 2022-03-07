#include "coinbase_websocket.h"

namespace cb
{
	coinbase_websocket_stream::coinbase_websocket_stream(std::shared_ptr<websocket_client> websocketClient)
		: websocket_stream{ websocketClient }
	{}

	void coinbase_websocket_stream::on_open()
	{

	}

	void coinbase_websocket_stream::on_close(std::string_view reason)
	{

	}

	void coinbase_websocket_stream::on_fail(std::string_view reason)
	{

	}

	void coinbase_websocket_stream::on_message(std::string_view message)
	{

	}

	void coinbase_websocket_stream::subscribe_order_book(const std::vector<tradable_pair>& tradablePairs)
	{

	}
}