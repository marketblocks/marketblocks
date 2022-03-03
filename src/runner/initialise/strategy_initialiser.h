#pragma once

#include <vector>

#include "trading/trading_options.h"
#include "exchanges/exchange.h"

namespace cb
{
	class strategy_initialiser
	{
	private:
		std::vector<std::shared_ptr<exchange>> _exchanges;
		trading_options _options;

	public:
		constexpr strategy_initialiser(std::vector<std::shared_ptr<exchange>> exchanges, trading_options options)
			: _exchanges{ std::move(exchanges) }, _options{ std::move(options) }
		{}

		constexpr std::vector<std::shared_ptr<exchange>> exchanges() const noexcept { return _exchanges; }
		constexpr trading_options options() const noexcept { return _options; }
	};
}