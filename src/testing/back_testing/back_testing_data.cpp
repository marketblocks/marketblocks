#include "back_testing_data.h"

namespace mb
{
	namespace internal
	{
		int get_data_index(const std::vector<timed_ohlcv_data>& data, std::time_t time)
		{
			for (int i = 0; i < data.size(); ++i)
			{
				if (data[i].time_stamp() == time)
				{
					return i;
				}
			}

			return -1;
		}

		std::optional<std::reference_wrapper<const timed_ohlcv_data>> get_data(const back_testing_data_source& dataSource, const tradable_pair& tradablePair)
		{
			const std::vector<timed_ohlcv_data>& pairData = dataSource.data().get_ohlcv_data(tradablePair);
			int index = get_data_index(pairData, dataSource.data_time());

			if (index != -1)
			{
				return std::reference_wrapper{ pairData.at(index) };
			}

			return std::nullopt;
		}
	}

	back_testing_data::back_testing_data(
		std::vector<tradable_pair> tradablePairs,
		std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>> data,
		std::time_t startTime,
		int interval,
		int size)
		: 
		_tradablePairs{ std::move(tradablePairs) }, 
		_data{ std::move(data) }, 
		_startTime{ startTime },
		_interval{ interval },
		_size{ size } 
	{}

	const std::vector<timed_ohlcv_data>& back_testing_data::get_ohlcv_data(const tradable_pair& pair) const
	{
		auto it = _data.find(pair);
		if (it != _data.end())
		{
			return it->second;
		}

		return {};
	}

	back_testing_data_source::back_testing_data_source(back_testing_data data)
		: _data{ std::move(data) }, _dataTime{ _data.start_time() }
	{}
}