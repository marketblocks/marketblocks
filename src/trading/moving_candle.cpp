#include "moving_candle.h"
#include "common/utils/financeutils.h"

namespace mb
{
	moving_candle::moving_candle(int interval)
		:
		_interval{ interval },
		_startTime{ 0 },
		_trades{}
	{}

	void moving_candle::update_values(std::time_t currentTime)
	{
		if (currentTime >= _startTime + _interval)
		{
			_startTime = currentTime - _interval;
		}

		while (_trades.size() > 1 && _trades[1].time_stamp() <= _startTime)
		{
			_trades.pop_front();
		}

		if (_trades.front().time_stamp() < _startTime)
		{
			trade_update fillTrade{ _startTime, _trades.front().price(), 0.0 };
			_trades.pop_front();
			_trades.push_front(std::move(fillTrade));
		}
	}

	void moving_candle::push_trade(trade_update trade)
	{
		if (_startTime == 0)
		{
			_startTime = trade.time_stamp();
		}

		_trades.push_back(std::move(trade));
		update_values(_trades.back().time_stamp());
	}

	ohlcv_data moving_candle::get_ohlcv(std::time_t time)
	{
		update_values(time);

		if (_trades.empty())
		{
			return ohlcv_data{};
		}

		double open{ _trades.front().price() };
		double close{ _trades.back().price() };
		double high{ open };
		double low{ open };
		double volume = 0.0;

		for (auto& trade : _trades)
		{
			high = std::max(high, trade.price());
			low = std::min(low, trade.price());
			volume += trade.volume();
		}

		return ohlcv_data{ _startTime, open, high, low, close, volume };
	}

	double moving_candle::get_quote_volume(std::time_t time)
	{
		update_values(time);

		double quoteVolume{ 0.0 };

		for (auto& trade : _trades)
		{
			quoteVolume += calculate_cost(trade.price(), trade.volume());
		}

		return quoteVolume;
	}
}