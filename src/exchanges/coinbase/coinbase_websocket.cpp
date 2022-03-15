#include "coinbase_websocket.h"

namespace cb::internal
{
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

	std::string coinbase_websocket_stream::get_order_book_subscription_message(const std::vector<tradable_pair>& tradablePairs) const
	{
		return "";
	}
}