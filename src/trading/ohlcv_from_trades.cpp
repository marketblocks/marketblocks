#include "ohlcv_from_trades.h"

namespace mb
{
	ohlcv_from_trades::ohlcv_from_trades(const ohlcv_data& startOhlcv, int interval)
		: 
		_startTime{ startOhlcv.time_stamp() },
		_open{ startOhlcv.open() },
		_high{ startOhlcv.high() },
		_low{ startOhlcv.low() },
		_close{ startOhlcv.close() },
		_volume{ startOhlcv.volume() },
		_interval{ interval },
		_lastTime{ _startTime }
	{}

	void ohlcv_from_trades::start_new_candle(std::time_t currentTime)
	{
		_startTime += ((currentTime - _startTime) / _interval) * _interval;
		_open = _close;
		_high = _close;
		_low = _close;
		_volume = 0.0;
	}

	void ohlcv_from_trades::add_trade(std::time_t time, double price, double volume)
	{
		if (time < _startTime)
		{
			return;
		}

		if (time >= _startTime + _interval)
		{
			start_new_candle(time);
		}

		if (time == _startTime)
		{
			_open = price;
		}

		_high = std::max(_high, price);
		_low = std::min(_low, price);
		_volume += volume;

		if (time > _lastTime)
		{
			_close = price;
			_lastTime = time;
		}
	}

	ohlcv_data ohlcv_from_trades::get_ohlcv(std::time_t currentTime)
	{
		assert(currentTime >= _startTime);

		if (currentTime >= _startTime + _interval)
		{
			start_new_candle(currentTime);
		}

		return ohlcv_data
		{
			_startTime, _open, _high, _low, _close, _volume
		};
	}
}