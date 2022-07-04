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
		W1,

		UNKNOWN
	};

	enum class subscription_status
	{
		UNSUBSCRIBED,
		SUBSCRIBED
	};

	ohlcv_interval parse_ohlcv_interval(std::string_view string);

	std::string to_string(ohlcv_interval interval);

	int to_seconds(ohlcv_interval interval);
}