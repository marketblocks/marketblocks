#pragma once

namespace cb
{
	class pair_stats
	{
	private:
		double _volume;
		double _low;
		double _high;
		double _open;

	public:
		constexpr pair_stats(
			double volume,
			double low,
			double high,
			double open)
			:
			_volume{ volume },
			_low{ low },
			_high{ high },
			_open{ open }
		{}

		constexpr double volume() const noexcept { return _volume; }
		constexpr double low() const noexcept { return _low; }
		constexpr double high() const noexcept { return _high; }
		constexpr double open() const noexcept { return _open; }
	};
}
