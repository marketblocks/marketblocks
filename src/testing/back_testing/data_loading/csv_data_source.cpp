#include "csv_data_source.h"
#include "logging/logger.h"
#include "common/exceptions/mb_exception.h"

namespace
{
	using namespace mb;

	class ohlcv_data_selector
	{
	private:
		int _stepSize;
		std::time_t _lastTime;

	public:
		ohlcv_data_selector(int stepSize)
			: _stepSize{ stepSize }, _lastTime{ -1 }
		{}

		bool operator()(const ohlcv_data& data)
		{
			if (_lastTime != -1 && data.time_stamp() - _lastTime < _stepSize)
			{
				return false;
			}

			_lastTime = data.time_stamp();
			return true;
		}
	};
}

namespace mb
{
	csv_data_source::csv_data_source(std::filesystem::path dataDirectory)
		: _dataDirectory{ std::move(dataDirectory) }
	{}

	std::vector<tradable_pair> csv_data_source::get_available_pairs()
	{
		std::vector<tradable_pair> pairs;

		for (const auto& directoryEntry : std::filesystem::directory_iterator(_dataDirectory))
		{
			if (!directoryEntry.is_regular_file() || directoryEntry.path().extension() != ".csv")
			{
				continue;
			}

			try
			{
				std::filesystem::path fileName{ directoryEntry.path().filename() };
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

	std::vector<ohlcv_data> csv_data_source::load_data(const tradable_pair& pair, int stepSize)
	{
		std::string pairName{ pair.to_string('_') };

		logger::instance().info("Loading data from {0}.csv", pairName);

		std::filesystem::path path = _dataDirectory / (pairName + ".csv");
		std::vector<ohlcv_data> data{
			read_csv_file<ohlcv_data>(path, ohlcv_data_selector{ stepSize }) };

		if (data.empty())
		{
			logger::instance().warning("Data for {} is invalid or empty", pairName);
		}

		std::sort(data.begin(), data.end());
		return data;
	}
}