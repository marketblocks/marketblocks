#pragma once

#include "ohlcv_data.h"

namespace mb
{
	class ohlcv_from_trades
	{
	private:
		std::time_t _startTime;
		double _open;
		double _high;
		double _low;
		double _close;
		double _volume;
		int _interval;
		std::time_t _lastTime;

		void start_new_candle(std::time_t currentTime);

	public:
		ohlcv_from_trades(const ohlcv_data& startOhlcv, int interval);

		void add_trade(std::time_t time, double price, double volume);
		ohlcv_data get_ohlcv(std::time_t currentTime);

		int interval() const noexcept { return _interval; }
	};
}