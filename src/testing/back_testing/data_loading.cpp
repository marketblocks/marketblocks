#include "data_loading.h"
#include "back_testing_data_source.h"
#include "common/csv/csv.h"
#include "common/file/file.h"
#include "common/utils/containerutils.h"
#include "logging/logger.h"
#include "trading/ohlcv_data.h"

namespace
{
	using namespace mb;

	std::vector<tradable_pair> get_available_pairs(std::string_view directory)
	{
		std::vector<tradable_pair> pairs;

		for (const auto& directoryEntry : std::filesystem::directory_iterator(directory))
		{
			if (!directoryEntry.is_regular_file() || directoryEntry.path().extension() != ".csv")
			{
				continue;
			}

			try
			{
				std::filesystem::path fileName = directoryEntry.path().filename();
				fileName.replace_extension();
				pairs.emplace_back(parse_tradable_pair(fileName.string(), '_'));
			}
			catch (const mb_exception& e)
			{
				continue;
			}
		}

		return pairs;
	}

	int calculate_time_steps(std::time_t startTime, std::time_t endTime, int stepSize)
	{
		return ((endTime - startTime) / stepSize) + 1;
	}
}

namespace mb
{
	back_testing_data load_back_testing_data(const back_testing_config& config)
	{
		std::vector<tradable_pair> pairs{ get_available_pairs(config.data_directory()) };
		std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>> ohlcvData;
		ohlcvData.reserve(pairs.size());

		int count = pairs.size();
		logger::instance().info("Found data for {} tradable pairs", count);

		if (config.dynamic_load())
		{
			logger::instance().info("Dynamic data loading is enabled");
			return back_testing_data
			{
				std::move(pairs),
				std::move(ohlcvData),
				config.start_time(),
				config.end_time(),
				config.step_size(),
				calculate_time_steps(config.start_time(), config.end_time(), config.step_size()),
				std::make_unique<back_testing_data_source>(config.data_directory(), config.step_size())
			};
		}

		std::time_t startTime = MAXLONGLONG;
		std::time_t endTime = 0;
		back_testing_data_source dataSource{ config.data_directory(), config.step_size() };

		for (auto& pair : pairs)
		{
			std::vector<timed_ohlcv_data> data{ dataSource.load_data(pair) };

			startTime = std::min(startTime, data.front().time_stamp());
			endTime = std::max(endTime, data.back().time_stamp());

			ohlcvData.emplace(pair, std::move(data));
		}

		startTime = std::max(startTime, config.start_time());

		if (config.end_time() != 0)
		{
			endTime = std::min(endTime, config.end_time());
		}

		return back_testing_data
		{
			std::move(pairs),
			std::move(ohlcvData),
			startTime,
			endTime,
			config.step_size(),
			calculate_time_steps(startTime, endTime, config.step_size())
		};
	}
}