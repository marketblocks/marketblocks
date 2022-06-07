#include "exchange_websocket_stream.h"
#include "logging/logger.h"

#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	exchange_websocket_stream::exchange_websocket_stream(std::string_view id, std::unique_ptr<websocket_connection> connection)
		: 
		_id{ std::move(id) },
		_connection{ std::move(connection) }
	{
		_connection->set_on_open_handler([this]() { on_open(); });
		_connection->set_on_close_handler([this](std::error_code error) { on_close(error); });
		_connection->set_on_fail_handler([this](std::error_code error) { on_fail(error); });
		_connection->set_on_message_handler([this](std::string_view message) { on_message(message); });

		connect();
	}

	void exchange_websocket_stream::on_open() const
	{

	}

	void exchange_websocket_stream::on_close(std::error_code error) const
	{

	}

	void exchange_websocket_stream::on_fail(std::error_code error) const
	{

	}

	order_book_state exchange_websocket_stream::get_order_book(const tradable_pair& pair, order_book_depth depth) const
	{
		throw not_implemented_exception{ "exchange_websocket_stream::get_order_book" };
	}

	double exchange_websocket_stream::get_price(const tradable_pair& pair) const
	{
		throw not_implemented_exception{ "exchange_websocket_stream::get_price" };
	}

	ohlcv_data exchange_websocket_stream::get_last_candle(const tradable_pair& pair, ohlcv_interval interval) const
	{
		throw not_implemented_exception{ "exchange_websocket_stream::get_last_candle" };
	}
}