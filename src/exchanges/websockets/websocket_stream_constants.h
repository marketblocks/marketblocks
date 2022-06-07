#pragma once

namespace mb
{
	enum class websocket_channel
	{
		ORDER_BOOK,
		PRICE,
		OHLCV
	};

	enum class ohlcv_interval
	{
		M1,
		M5,
		M15,
		H1,
		D1,
		W1
	};

	enum class order_book_depth
	{
		DEPTH_1,
		DEPTH_10,
		DEPTH_100,
		DEPTH_MAX
	};
}