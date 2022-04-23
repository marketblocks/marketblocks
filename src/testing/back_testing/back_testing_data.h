#pragma once

#include <vector>
#include <unordered_map>

#include "trading/tradable_pair.h"
#include "trading/historical_trade.h"

namespace mb
{
	class back_testing_data
	{
	private:
		std::vector<tradable_pair> _tradablePairs;
		std::unordered_map<tradable_pair, std::vector<historical_trade>> _trades;

	public:
		back_testing_data(
			std::vector<tradable_pair> tradablePairs,
			std::unordered_map<tradable_pair, std::vector<historical_trade>> trades);

		const std::vector<tradable_pair>& tradable_pairs() const noexcept { return _tradablePairs; }
		const std::vector<historical_trade>& get_trades(const tradable_pair& pair) const;
	};
}