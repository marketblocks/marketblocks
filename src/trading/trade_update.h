#pragma once

#include <ctime>

namespace mb
{
	class trade_update
	{
	private:
		std::time_t _timeStamp;
		double _price;
		double _volume;

	public:
		constexpr trade_update(std::time_t timeStamp, double price, double volume)
			: _timeStamp{ timeStamp }, _price{ price }, _volume{ volume }
		{}

		constexpr std::time_t time_stamp() const noexcept { return _timeStamp; }
		constexpr double price() const noexcept { return _price; }
		constexpr double volume() const noexcept { return _volume; }
	};
}