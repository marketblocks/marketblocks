#pragma once

#include "websocket_stream_constants.h"
#include "trading/tradable_pair.h"
#include "trading/trade_update.h"
#include "trading/ohlcv_data.h"
#include "trading/order_book.h"

namespace mb
{
	class trade_update_message
	{
	private:
		tradable_pair _pair;
		trade_update _trade;

	public:
		trade_update_message(tradable_pair pair, trade_update trade)
			: _pair{ std::move(pair) }, _trade{ std::move(trade) }
		{}

		const tradable_pair& pair() const noexcept { return _pair; }
		const trade_update& trade() const noexcept { return _trade; }
	};

	class ohlcv_update_message
	{
	private:
		tradable_pair _pair;
		ohlcv_interval _interval;
		ohlcv_data _ohlcv;

	public:
		ohlcv_update_message(tradable_pair pair, ohlcv_interval interval, ohlcv_data data)
			: _pair{ std::move(pair) }, _interval{ interval }, _ohlcv{ std::move(data) }
		{}

		const tradable_pair& pair() const noexcept { return _pair; }
		ohlcv_interval interval() const noexcept { return _interval; }
		const ohlcv_data& ohlcv() const noexcept { return _ohlcv; }
	};

	class order_book_update_message
	{
	private:
		tradable_pair _pair;
		order_book_entry _entry;

	public:
		order_book_update_message(tradable_pair pair, order_book_entry entry)
			: _pair{ std::move(pair) }, _entry{ std::move(entry) }
		{}

		const tradable_pair& pair() const noexcept { return _pair; }
		const order_book_entry& entry() const noexcept { return _entry; }
	};
}