#include "websocket_stream.h"

namespace mb
{
	void websocket_stream::set_trade_update_handler(trade_update_handler handler)
	{
		_tradeUpdateHandler = std::move(handler);
	}

	void websocket_stream::set_ohlcv_update_handler(ohlcv_update_handler handler)
	{
		_ohlcvUpdateHandler = std::move(handler);
	}

	void websocket_stream::set_order_book_update_handler(order_book_update_handler handler)
	{
		_orderBookUpdateHandler = std::move(handler);
	}

	bool websocket_stream::has_trade_update_handler()
	{
		return _tradeUpdateHandler.operator bool();
	}

	bool websocket_stream::has_ohlcv_update_handler()
	{
		return _ohlcvUpdateHandler.operator bool();
	}

	bool websocket_stream::has_order_book_update_handler()
	{
		return _orderBookUpdateHandler.operator bool();
	}

	void websocket_stream::fire_trade_update(trade_update_message message)
	{
		if (_tradeUpdateHandler)
		{
			_tradeUpdateHandler(std::move(message));
		}
	}

	void websocket_stream::fire_ohlcv_update(ohlcv_update_message message)
	{
		if (_ohlcvUpdateHandler)
		{
			_ohlcvUpdateHandler(std::move(message));
		}
	}

	void websocket_stream::fire_order_book_update(order_book_update_message message)
	{
		if (_orderBookUpdateHandler)
		{
			_orderBookUpdateHandler(std::move(message));
		}
	}
}