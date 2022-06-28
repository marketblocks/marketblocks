#pragma once

#include <filesystem>

#include "back_testing_data_source.h"

namespace mb
{
	class csv_data_source : public back_testing_data_source
	{
	private:
		std::filesystem::path _dataDirectory;

	public:
		csv_data_source(std::filesystem::path dataDirectory);

		std::vector<tradable_pair> get_available_pairs() override;
		std::vector<ohlcv_data> load_data(const tradable_pair& pair, int stepSize) override;
	};
}