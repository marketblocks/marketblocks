#pragma once

#include <queue>

#include "trade_update.h"
#include "ohlcv_data.h"

namespace mb
{
	class moving_candle
	{
	private:
		int _interval;
		std::deque<trade_update> _trades;
		std::time_t _startTime;

		void update_values(std::time_t currentTime);

	public:
		moving_candle(int interval);

		void push_trade(trade_update trade);
		ohlcv_data get_ohlcv(std::time_t time);
		double get_quote_volume(std::time_t time);
	};
}