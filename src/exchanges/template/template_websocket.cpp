#include "template_websocket.h"
#include "exchanges/exchange_ids.h"

namespace mb::internal
{
	template_websocket_stream::template_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory)
		: 
		exchange_websocket_stream
		{ 
			exchange_ids::BYBIT,
			"wss://", 
			'\0',
			std::move(connectionFactory)
		}
	{}

	void template_websocket_stream::process_trade_message(const json_document& json)
	{
	}

	void template_websocket_stream::on_message(std::string_view message)
	{
	}

	void template_websocket_stream::send_subscribe(const websocket_subscription& subscription)
	{

	}

	void template_websocket_stream::send_unsubscribe(const websocket_subscription& subscription)
	{

	}
}