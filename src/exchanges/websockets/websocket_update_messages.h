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
		constexpr trade_update_message(tradable_pair pair, trade_update trade)
			: _pair{ std::move(pair) }, _trade{ std::move(trade) }
		{}

		constexpr const tradable_pair& pair() const noexcept { return _pair; }
		constexpr const trade_update& trade() const noexcept { return _trade; }
	};

	class ohlcv_update_message
	{
	private:
		tradable_pair _pair;
		ohlcv_interval _interval;
		ohlcv_data _ohlcv;

	public:
		constexpr ohlcv_update_message(tradable_pair pair, ohlcv_interval interval, ohlcv_data data)
			: _pair{ std::move(pair) }, _interval{ interval }, _ohlcv{ std::move(data) }
		{}

		constexpr const tradable_pair& pair() const noexcept { return _pair; }
		constexpr ohlcv_interval interval() const noexcept { return _interval; }
		constexpr const ohlcv_data& ohlcv() const noexcept { return _ohlcv; }
	};

	class order_book_update_message
	{
	private:
		tradable_pair _pair;
		order_book_state _orderBook;

	public:
		constexpr order_book_update_message(tradable_pair pair, order_book_state orderBook)
			: _pair{ std::move(pair) }, _orderBook{ std::move(orderBook) }
		{}

		constexpr const tradable_pair& pair() const noexcept { return _pair; }
		constexpr const order_book_state& order_book() const noexcept { return _orderBook; }
	};
}