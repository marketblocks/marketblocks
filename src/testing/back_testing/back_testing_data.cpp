#include "back_testing_data.h"

namespace mb
{
	namespace internal
	{
		int get_data_index(const std::vector<timed_ohlcv_data>& data, std::time_t time)
		{
			constexpr int NOT_FOUND = -1;

			if (data.empty() || time < data.front().time_stamp())
			{
				return NOT_FOUND;
			}

			if (time > data.back().time_stamp())
			{
				return data.size() - 1;
			}

			for (int i = 0; i < data.size(); ++i)
			{
				std::time_t timeStamp = data[i].time_stamp();

				if (timeStamp > time)
				{
					return i - 1;
				}

				if (timeStamp == time)
				{
					return i;
				}
			}

			return NOT_FOUND;
		}

		std::optional<std::reference_wrapper<const timed_ohlcv_data>> get_data(const back_testing_data_navigator& dataNavigator, const tradable_pair& tradablePair)
		{
			const std::vector<timed_ohlcv_data>& pairData = dataNavigator.data().get_ohlcv_data(tradablePair);
			int index = get_data_index(pairData, dataNavigator.data_time());

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
		std::time_t endTime,
		int step_size,
		int size)
		: 
		_tradablePairs{ std::move(tradablePairs) }, 
		_data{ std::move(data) }, 
		_startTime{ startTime },
		_endTime{ endTime },
		_stepSize{ step_size },
		_timeSteps{ size } 
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

	back_testing_data_navigator::back_testing_data_navigator(back_testing_data data)
		: _data{ std::move(data) }, _dataTime{ _data.start_time() }
	{}
}