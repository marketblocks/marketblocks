#pragma once

#include <ctime>

#include "common/csv/csv.h"

namespace mb
{
	class ohlcv_data
	{
	private:
		std::time_t _timeStamp;
		double _open;
		double _high;
		double _low;
		double _close;
		double _volume;

	public:
		constexpr ohlcv_data()
			:
			_timeStamp{ -1 },
			_open{},
			_high{},
			_low{},
			_close{},
			_volume{}
		{}

		constexpr ohlcv_data(
			std::time_t timeStamp,
			double open,
			double high,
			double low,
			double close,
			double volume)
			:
			_timeStamp{ timeStamp },
			_open{ open },
			_high{ high },
			_low{ low },
			_close{ close },
			_volume{ volume }
		{}

		constexpr std::time_t time_stamp() const noexcept { return _timeStamp; }
		constexpr double open() const noexcept { return _open; }
		constexpr double high() const noexcept { return _high; }
		constexpr double low() const noexcept { return _low; }
		constexpr double close() const noexcept { return _close; }
		constexpr double volume() const noexcept { return _volume; }

		constexpr bool operator<(const ohlcv_data& other) const
		{
			return _timeStamp < other._timeStamp;
		}
	};

	template<>
	ohlcv_data from_csv_row(const csv_row& row);
}