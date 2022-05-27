#include "back_testing_data.h"
#include "common/utils/containerutils.h"

namespace mb
{
	back_testing_data::back_testing_data(
		std::vector<tradable_pair> tradablePairs,
		std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>> data,
		std::time_t startTime,
		std::time_t endTime,
		int step_size,
		int size,
		std::unique_ptr<back_testing_data_source> dataSource)
		: 
		_tradablePairs{ std::move(tradablePairs) }, 
		_data{ std::move(data) }, 
		_startTime{ startTime },
		_endTime{ endTime },
		_stepSize{ step_size },
		_timeSteps{ size },
		_dataSource{ std::move(dataSource) }
	{}

	const std::vector<timed_ohlcv_data>& back_testing_data::get_ohlcv_data(const tradable_pair& pair)
	{
		auto it = _data.find(pair);
		if (it != _data.end())
		{
			return it->second;
		}

		if (_dataSource)
		{
			std::vector<timed_ohlcv_data> data = _dataSource->load_data(pair);
			return _data[pair] = std::move(data);
		}

		return _data[pair] = {};
	}

	back_testing_data_navigator::back_testing_data_navigator(back_testing_data data)
		: _data{ std::move(data) }, _dataTime{ _data.start_time() }, _iteratorCache{}
	{}

	std::vector<timed_ohlcv_data>::const_iterator back_testing_data_navigator::find_data_position(const std::vector<timed_ohlcv_data>& pairData, const tradable_pair& tradablePair)
	{
		if (pairData.empty() || _dataTime < pairData.front().time_stamp())
		{
			return pairData.end();
		}

		if (_dataTime > pairData.back().time_stamp())
		{
			return std::prev(pairData.end());
		}

		auto it = find_or_default(_iteratorCache, tradablePair, pairData.begin());

		for (it; it != pairData.end(); ++it)
		{
			std::time_t timeStamp = it->time_stamp();

			if (timeStamp > _dataTime)
			{
				--it;
				break;
			}

			if (timeStamp == _dataTime)
			{
				break;
			}
		}

		_iteratorCache[tradablePair] = it;
		return it;
	}

	std::optional<std::reference_wrapper<const timed_ohlcv_data>> back_testing_data_navigator::find_data_point(const tradable_pair& tradablePair)
	{
		const std::vector<timed_ohlcv_data>& pairData{ _data.get_ohlcv_data(tradablePair) };
		auto iterator = find_data_position(pairData, tradablePair);

		if (iterator != pairData.end())
		{
			return std::reference_wrapper{ *iterator };
		}

		return std::nullopt;
	}
}