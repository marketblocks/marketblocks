#include "websocket_stream.h"

namespace
{
	template<typename Handler, typename Message>
	void fire_handlers(std::vector<Handler>& handlers, Message message)
	{
		for (auto& handler : handlers)
		{
			handler(message);
		}
	}
}

namespace mb
{
	void websocket_stream::add_trade_update_handler(trade_update_handler handler)
	{
		_tradeUpdateHandlers.emplace_back(std::move(handler));
	}

	void websocket_stream::add_ohlcv_update_handler(ohlcv_update_handler handler)
	{
		_ohlcvUpdateHandlers.emplace_back(std::move(handler));
	}

	void websocket_stream::add_order_book_update_handler(order_book_update_handler handler)
	{
		_orderBookUpdateHandlers.emplace_back(std::move(handler));
	}

	bool websocket_stream::has_trade_update_handler()
	{
		return !_tradeUpdateHandlers.empty();
	}

	bool websocket_stream::has_ohlcv_update_handler()
	{
		return !_ohlcvUpdateHandlers.empty();
	}

	bool websocket_stream::has_order_book_update_handler()
	{
		return !_orderBookUpdateHandlers.empty();
	}

	void websocket_stream::fire_trade_update(trade_update_message message)
	{
		if (has_trade_update_handler())
		{
			fire_handlers(_tradeUpdateHandlers, message);
		}
	}

	void websocket_stream::fire_ohlcv_update(ohlcv_update_message message)
	{
		if (has_ohlcv_update_handler())
		{
			fire_handlers(_ohlcvUpdateHandlers, message);
		}
	}

	void websocket_stream::fire_order_book_update(order_book_update_message message)
	{
		if (has_order_book_update_handler())
		{
			fire_handlers(_orderBookUpdateHandlers, message);
		}
	}
}