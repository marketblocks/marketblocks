#pragma once

#include <vector>
#include <unordered_map>

#include "trading/tradable_pair.h"
#include "trading/ohlcv_data.h"

namespace mb
{
	class back_testing_data
	{
	private:
		std::vector<tradable_pair> _tradablePairs;
		std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>> _data;

	public:
		back_testing_data(
			std::vector<tradable_pair> tradablePairs,
			std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>> data);

		const std::vector<tradable_pair>& tradable_pairs() const noexcept { return _tradablePairs; }
		const std::vector<timed_ohlcv_data>& data(const tradable_pair& pair) const;
	};
}