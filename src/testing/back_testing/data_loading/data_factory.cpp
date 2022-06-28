#include "data_factory.h"
#include "back_testing_data_source.h"
#include "csv_data_source.h"
#include "common/file/file.h"
#include "common/utils/containerutils.h"
#include "logging/logger.h"
#include "trading/ohlcv_data.h"

namespace
{
	using namespace mb;

	std::unique_ptr<back_testing_data_source> create_data_source(const back_testing_config& config)
	{
		return std::make_unique<csv_data_source>(config.data_directory());
	}

	int calculate_time_steps(std::time_t startTime, std::time_t endTime, int stepSize)
	{
		return ((endTime - startTime) / stepSize) + 1;
	}

	void load_data(
		back_testing_data_source* dataSource,
		const back_testing_config& config, 
		const std::vector<tradable_pair>& pairs, 
		std::unordered_map<tradable_pair, std::vector<ohlcv_data>>& ohlcvData,
		std::time_t& startTime,
		std::time_t& endTime)
	{
		startTime = MAXLONGLONG;
		endTime = 0;

		for (auto& pair : pairs)
		{
			std::vector<ohlcv_data> data{ dataSource->load_data(pair, config.step_size()) };

			startTime = std::min(startTime, data.front().time_stamp());
			endTime = std::max(endTime, data.back().time_stamp());

			ohlcvData.emplace(pair, std::move(data));
		}

		startTime = std::max(startTime, config.start_time());

		if (config.end_time() != 0)
		{
			endTime = std::min(endTime, config.end_time());
		}
	}
}

namespace mb
{
	std::shared_ptr<back_testing_data> load_back_testing_data(const back_testing_config& config)
	{
		std::unique_ptr<back_testing_data_source> dataSource{ create_data_source(config) };

		std::vector<tradable_pair> pairs{ dataSource->get_available_pairs() };
		std::unordered_map<tradable_pair, std::vector<ohlcv_data>> ohlcvData;
		ohlcvData.reserve(pairs.size());

		int count = pairs.size();
		logger::instance().info("Found data for {} tradable pairs", count);

		std::time_t startTime = config.start_time();
		std::time_t endTime = config.end_time();

		if (config.dynamic_load())
		{
			logger::instance().info("Dynamic data loading is enabled");
		}
		else
		{
			load_data(dataSource.get(), config, pairs, ohlcvData, startTime, endTime);
			dataSource = nullptr;
		}

		return std::make_shared<back_testing_data>(
			std::move(pairs),
			std::move(ohlcvData),
			startTime,
			endTime,
			config.step_size(),
			calculate_time_steps(startTime, endTime, config.step_size()),
			std::move(dataSource));
	}
}