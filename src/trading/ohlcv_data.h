#pragma once

#include <ctime>

#include "common/csv/csv.h"

namespace mb
{
	class ohlcv_data
	{
	private:
		double _open;
		double _high;
		double _low;
		double _close;
		double _volume;

	public:
		constexpr ohlcv_data(
			double open,
			double high,
			double low,
			double close,
			double volume)
			:
			_open{ open },
			_high{ high },
			_low{ low },
			_close{ close },
			_volume{ volume }
		{}

		constexpr double open() const noexcept { return _open; }
		constexpr double high() const noexcept { return _high; }
		constexpr double low() const noexcept { return _low; }
		constexpr double close() const noexcept { return _close; }
		constexpr double volume() const noexcept { return _volume; }
	};

	class timed_ohlcv_data
	{
	private:
		std::time_t _timeStamp;
		ohlcv_data _data;

	public:
		constexpr timed_ohlcv_data(std::time_t timeStamp, ohlcv_data data)
			: _timeStamp{ timeStamp }, _data{ std::move(data) }
		{}

		constexpr std::time_t time_stamp() const noexcept { return _timeStamp; }
		constexpr const ohlcv_data& data() const noexcept { return _data; }

		constexpr bool operator<(const timed_ohlcv_data& other) const
		{
			return _timeStamp < other._timeStamp;
		}
	};

	template<>
	timed_ohlcv_data from_csv_row(const csv_row& row);
}