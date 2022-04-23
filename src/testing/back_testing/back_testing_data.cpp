#include "back_testing_data.h"

namespace mb
{
	back_testing_data::back_testing_data(
		std::vector<tradable_pair> tradablePairs,
		std::unordered_map<tradable_pair, std::vector<historical_trade>> trades)
	: _tradablePairs{ std::move(tradablePairs) }, _trades{ std::move(trades) }
	{}

	const std::vector<historical_trade>& back_testing_data::get_trades(const tradable_pair& pair) const
	{
		auto it = _trades.find(pair);
		if (it != _trades.end())
		{
			return it->second;
		}

		return {};
	}
}