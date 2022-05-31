#include "template_websocket.h"

namespace mb::internal
{
	void template_websocket_stream::on_open()
	{

	}

	void template_websocket_stream::on_close(std::string_view reason)
	{

	}

	void template_websocket_stream::on_fail(std::string_view reason)
	{

	}

	void template_websocket_stream::on_message(std::string_view message)
	{

	}

	std::string template_websocket_stream::get_order_book_subscription_message(const std::vector<tradable_pair>& tradablePairs) const
	{
		return "";
	}

	std::string template_websocket_stream::get_order_book_unsubscription_message(const std::vector<tradable_pair>& tradablePairs) const
	{
		return "";
	}
}