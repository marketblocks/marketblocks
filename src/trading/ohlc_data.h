#pragma once

namespace mb
{
	class ohlc_data
	{
	private:
		double _open;
		double _high;
		double _low;
		double _close;
		double _volume;

	public:
		constexpr ohlc_data(
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
}
