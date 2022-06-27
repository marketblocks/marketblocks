#pragma once

#include "common/csv/csv.h"
#include "trading/ohlcv_data.h"
#include "trading/tradable_pair.h"

namespace mb
{
	class back_testing_data_source
	{
	private:
		std::filesystem::path _dataDirectory;
		int _stepSize;

	public:
		back_testing_data_source(std::filesystem::path dataDirectory, int stepSize);

		std::vector<ohlcv_data> load_data(const tradable_pair& pair) const;
	};
}