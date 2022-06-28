#pragma once

#include "trading/ohlcv_data.h"
#include "trading/tradable_pair.h"

namespace mb
{
	class back_testing_data_source
	{
	public:
		virtual ~back_testing_data_source() = default;

		virtual std::vector<tradable_pair> get_available_pairs() = 0;
		virtual std::vector<ohlcv_data> load_data(const tradable_pair& pair, int stepSize) = 0;
	};
}