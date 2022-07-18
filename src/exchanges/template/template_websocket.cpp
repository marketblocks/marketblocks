#include "template_websocket.h"
#include "exchanges/exchange_ids.h"

namespace mb::internal
{
	template_websocket_stream::template_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory)
		: exchange_websocket_stream{ exchange_ids::BYBIT, "wss://", std::move(connectionFactory) }
	{}

	std::string template_websocket_stream::generate_subscription_id(const unique_websocket_subscription& subscription) const
	{
		return "";
	}

	void template_websocket_stream::process_price_message(const json_document& json)
	{
	}

	void template_websocket_stream::on_message(std::string_view message)
	{
	}

	void template_websocket_stream::subscribe(const websocket_subscription& subscription)
	{

	}

	void template_websocket_stream::unsubscribe(const websocket_subscription& subscription)
	{

	}
}