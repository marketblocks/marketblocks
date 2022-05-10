#include "data_loading.h"
#include "common/csv/csv.h"
#include "common/file/file.h"
#include "common/utils/containerutils.h"
#include "logging/logger.h"
#include "trading/ohlcv_data.h"

namespace
{
	using namespace mb;

	void sort_and_filter(std::vector<timed_ohlcv_data>& data, std::time_t startTime, int interval)
	{
		std::sort(data.begin(), data.end());

		if (startTime != 0)
		{
			auto dataCutoff = find<timed_ohlcv_data>(data, [startTime](const timed_ohlcv_data& trade) { return trade.time_stamp() > startTime; });
			data.erase(data.begin(), std::prev(dataCutoff));
		}
	}
}

namespace mb
{
	back_testing_data load_back_testing_data(const back_testing_config& config)
	{
		std::vector<tradable_pair> pairs;
		std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>> ohlcvData;
		int size = 0;
		std::time_t startTime = MAXLONGLONG;
		tradable_pair pair{ "abc", "def" };

		for (const auto& directoryEntry : std::filesystem::directory_iterator(config.data_directory()))
		{
			if (!directoryEntry.is_regular_file() || directoryEntry.path().extension() != ".csv")
			{
				continue;
			}

			try
			{
				std::filesystem::path fileName = directoryEntry.path().filename();
				fileName.replace_extension();
				pair = parse_tradable_pair(fileName.string(), '_');
			}
			catch (const mb_exception& e)
			{
				continue;
			}

			std::vector<timed_ohlcv_data> data{ read_csv_file<timed_ohlcv_data>(directoryEntry.path()) };

			if (data.empty())
			{
				continue;
			}

			sort_and_filter(data, config.start_time(), config.step_size());

			size = std::max<int>(size, data.size());
			startTime = std::min(startTime, data.front().time_stamp());

			pairs.push_back(pair);
			ohlcvData.emplace(pair, std::move(data));
		}

		return back_testing_data
		{
			std::move(pairs),
			std::move(ohlcvData),
			startTime,
			config.step_size(),
			size
		};
	}
}