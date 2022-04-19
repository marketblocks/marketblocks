#pragma once

#include <chrono>

#include "trading_constants.h"

namespace mb
{
	class historical_trade
	{
	private:
		std::time_t _timeStamp;
		trade_action _action;
		double _assetPrice;
		double _volume;

	public:
		constexpr historical_trade(
			std::time_t timeStamp,
			trade_action action,
			double assetPrice,
			double volume)
			:
			_timeStamp{ timeStamp },
			_action{ std::move(action) },
			_assetPrice{ assetPrice },
			_volume{ volume }
		{}

		constexpr time_t time_stamp() const noexcept { return _timeStamp; }
		constexpr const trade_action& action() const noexcept { return _action; }
		constexpr double asset_price() const noexcept { return _assetPrice; }
		constexpr double volume() const noexcept { return _volume; }
	};
}
