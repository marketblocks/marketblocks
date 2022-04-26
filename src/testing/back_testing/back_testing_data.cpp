#include "back_testing_data.h"

namespace mb
{
	back_testing_data::back_testing_data(
		std::vector<tradable_pair> tradablePairs,
		std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>> data)
	: _tradablePairs{ std::move(tradablePairs) }, _data{ std::move(data) }
	{}

	const std::vector<timed_ohlcv_data>& back_testing_data::data(const tradable_pair& pair) const
	{
		auto it = _data.find(pair);
		if (it != _data.end())
		{
			return it->second;
		}

		return {};
	}
}