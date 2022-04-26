#include "data_loading.h"
#include "common/csv/csv.h"
#include "common/file/file.h"
#include "common/utils/containerutils.h"
#include "logging/logger.h"
#include "trading/ohlcv_data.h"

namespace
{
	using namespace mb;

	std::vector<tradable_pair> parse_tradable_pairs(const std::vector<std::string>& pairNames)
	{
		std::vector<tradable_pair> pairs;
		pairs.reserve(pairNames.size());

		logger& log{ logger::instance() };

		for (auto& name : pairNames)
		{
			try
			{
				pairs.emplace_back(parse_tradable_pair(name));
			}
			catch (const mb_exception& e)
			{
				log.warning(e.what());
			}
		}

		return pairs;
	}

	std::vector<timed_ohlcv_data> load_cached_data(const std::filesystem::path& directory, const tradable_pair& pair, std::time_t startTime)
	{
		logger::instance().info("Loading data for {}...", pair.to_string('/'));

		std::filesystem::path path{ directory };
		path.append(pair.to_string());
		path.replace_extension("csv");

		if (std::filesystem::exists(path))
		{
			return read_csv_file<timed_ohlcv_data>(path);
		}

		logger::instance().warning("Back-testing data file {} does not exist", path.generic_string());
		return {};
	}

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
		std::vector<tradable_pair> pairs{ parse_tradable_pairs(config.pairs()) };
		std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>> ohlcvData;

		for (auto& pair : pairs)
		{
			std::vector<timed_ohlcv_data> data{ load_cached_data(config.data_directory(), pair, config.start_time())};
			sort_and_filter(data, config.start_time(), config.step_size());

			ohlcvData.emplace(pair, std::move(data));
		}

		return back_testing_data{ std::move(pairs), std::move(ohlcvData) };
	}
}