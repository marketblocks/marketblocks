#include "data_loading.h"
#include "common/csv/csv.h"
#include "common/file/file.h"
#include "common/utils/containerutils.h"
#include "logging/logger.h"
#include "trading/ohlcv_data.h"

namespace
{
	using namespace mb;

	class timed_ohlcv_data_csv_selector
	{
	private:
		int _stepSize;
		std::time_t _lastTime;

	public:
		timed_ohlcv_data_csv_selector(int stepSize)
			: _stepSize{ stepSize }, _lastTime{ -1 }
		{}

		bool operator()(const timed_ohlcv_data& data)
		{
			if (data.time_stamp() - _lastTime < _stepSize)
			{
				return false;
			}

			_lastTime = data.time_stamp();
			return true;
		}
	};

	void sort_and_filter(std::vector<timed_ohlcv_data>& data, std::time_t startTime, int step_size)
	{
		std::sort(data.begin(), data.end());
	}

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
}

namespace mb
{
	back_testing_data load_back_testing_data(const back_testing_config& config)
	{
		std::vector<tradable_pair> pairs{ get_available_pairs(config.data_directory()) };
		std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>> ohlcvData;
		ohlcvData.reserve(pairs.size());

		std::time_t startTime = MAXLONGLONG;
		std::time_t endTime = 0;

		int count = pairs.size();
		logger::instance().info("Found data for {} tradable pairs", count);

		for (int i = 0; i < count; ++i)
		{
			std::string pairName{ pairs[i].to_string('_') };

			logger::instance().info("Loading data for {0} ({1}/{2})", pairName, i + 1, count);

			std::filesystem::path path = config.data_directory();
			path /= pairName;
			path.replace_extension(".csv");

			std::vector<timed_ohlcv_data> data{
				read_csv_file<timed_ohlcv_data>(path, timed_ohlcv_data_csv_selector{config.step_size()}) };

			if (data.empty())
			{
				logger::instance().warning("Data for {} is invalid or empty", pairName);
				continue;
			}

			std::sort(data.begin(), data.end());

			startTime = std::min(startTime, data.front().time_stamp());
			endTime = std::max(endTime, data.back().time_stamp());

			ohlcvData.emplace(pairs[i], std::move(data));
		}

		if (ohlcvData.empty())
		{
			throw mb_exception{ "Failed to load any back testing data" };
		}

		startTime = std::max(startTime, config.start_time());
		int timeSteps = ((endTime - startTime) / config.step_size()) + 1;

		return back_testing_data
		{
			std::move(pairs),
			std::move(ohlcvData),
			startTime,
			endTime,
			config.step_size(),
			timeSteps
		};
	}
}