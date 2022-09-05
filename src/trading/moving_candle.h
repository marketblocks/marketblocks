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
		int _offset;
		std::deque<trade_update> _trades;
		std::time_t _startTime;

		void update_values(std::time_t currentTime);
		std::vector<trade_update> get_offset_trades(std::time_t currentTime) const;

	public:
		moving_candle(int interval, int offset = 0);

		void push_trade(trade_update trade);
		ohlcv_data get_ohlcv(std::time_t time);
		double get_quote_volume(std::time_t time);

		void reset();
	};
}