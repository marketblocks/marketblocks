#pragma once

#include <string>

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
}