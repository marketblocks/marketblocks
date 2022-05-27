#include "back_testing_data_source.h"
#include "logging/logger.h"

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
}

namespace mb
{
	back_testing_data_source::back_testing_data_source(std::filesystem::path dataDirectory, int stepSize)
		: _dataDirectory{ std::move(dataDirectory) }, _stepSize{ stepSize }
	{}

	std::vector<timed_ohlcv_data> back_testing_data_source::load_data(const tradable_pair& pair) const
	{
		std::string pairName{ pair.to_string('_') };

		logger::instance().info("Loading data from {0}.csv", pairName);

		std::filesystem::path path = _dataDirectory / (pairName + ".csv");
		std::vector<timed_ohlcv_data> data{
			read_csv_file<timed_ohlcv_data>(path, timed_ohlcv_data_csv_selector{ _stepSize }) };

		if (data.empty())
		{
			logger::instance().warning("Data for {} is invalid or empty", pairName);
		}

		std::sort(data.begin(), data.end());
		return data;
	}
}