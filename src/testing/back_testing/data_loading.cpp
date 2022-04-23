#include "data_loading.h"
#include "trading/historical_trade.h"
#include "common/file/file.h"
#include "common/utils/containerutils.h"
#include "logging/logger.h"

namespace
{
	using namespace mb;

	std::filesystem::path get_path(std::string_view directory, std::string_view exchange, std::string_view pair)
	{
		std::filesystem::path path{ directory };
		path.append(exchange);
		path.append(pair);
		path.replace_extension("csv");

		return path;
	}

	std::vector<historical_trade> load_cached_data(const std::filesystem::path& path, std::time_t startTime)
	{
		if (std::filesystem::exists(path))
		{
			std::vector<historical_trade> allData{ read_csv_file<historical_trade>(path) };
			std::sort(allData.begin(), allData.end());

			if (startTime != 0)
			{
				auto dataCutoff = find<historical_trade>(allData, [startTime](const historical_trade& trade) { return trade.time_stamp() >= startTime; });
				allData.erase(allData.begin(), dataCutoff);
			}
		}

		return {};
	}

	void cache_new_data(const std::filesystem::path& path, const std::vector<historical_trade>& data)
	{
		write_to_csv_file<historical_trade>(path, data);
	}

	std::unordered_set<tradable_pair> parse_tradable_pairs(const std::vector<std::string>& pairNames)
	{
		std::unordered_set<tradable_pair> pairs;
		pairs.reserve(pairNames.size());

		logger& log{ logger::instance() };

		for (auto& name : pairNames)
		{
			try
			{
				pairs.emplace(parse_tradable_pair(name));
			}
			catch (const mb_exception& e)
			{
				log.warning(e.what());
			}
		}

		return pairs;
	}

	std::vector<tradable_pair> get_tradable_pairs(const std::vector<std::string>& includedPairs, const std::vector<std::string>& excludedPairs, std::shared_ptr<exchange> exchange)
	{
		std::unordered_set<tradable_pair> pairs = includedPairs.empty()
			? std::unordered_set<tradable_pair>{ to_unordered_set<tradable_pair>(exchange->get_tradable_pairs()) }
			: parse_tradable_pairs(includedPairs);

		if (!excludedPairs.empty())
		{
			std::unordered_set<tradable_pair> excluded = parse_tradable_pairs(excludedPairs);

			for (auto& pair : excluded)
			{
				pairs.erase(pair);
			}
		}

		return to_vector<tradable_pair>(pairs);
	}
}

namespace mb
{
	back_testing_data load_back_testing_data(const back_testing_config& config, std::shared_ptr<exchange> exchange)
	{
		std::vector<tradable_pair> pairs{ get_tradable_pairs(config.included_pairs(), config.excluded_pairs(), exchange) };
		std::unordered_map<tradable_pair, std::vector<historical_trade>> tradeData;

		for (auto& pair : pairs)
		{
			std::filesystem::path dataPath{ get_path(config.data_path(), exchange->id(), pair.to_string('_')) };
			std::vector<historical_trade> data{ load_cached_data(dataPath, config.start_time()) };
			std::sort(data.begin(), data.end());

			if (config.refresh_data())
			{
				std::time_t start = data.empty() ? 0 : data.back().time_stamp();
				std::vector<historical_trade> newData{ exchange->get_historical_trades(pair, start) };

				cache_new_data(dataPath, newData);

				data.insert(data.end(), newData.begin(), newData.end());
			}

			tradeData.emplace(pair, std::move(data));
		}

		return back_testing_data{ std::move(pairs), std::move(tradeData) };
	}
}