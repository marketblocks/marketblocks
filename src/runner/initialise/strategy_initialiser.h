#pragma once

#include <vector>

#include "exchanges/exchange.h"

namespace mb
{
	class strategy_initialiser
	{
	private:
		std::vector<std::shared_ptr<exchange>> _exchanges;

	public:
		constexpr strategy_initialiser(std::vector<std::shared_ptr<exchange>> exchanges)
			: _exchanges{ std::move(exchanges) }
		{}

		constexpr std::vector<std::shared_ptr<exchange>> exchanges() const noexcept { return _exchanges; }
	};
}