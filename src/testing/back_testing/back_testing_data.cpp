#include "back_testing_data.h"
#include "common/utils/containerutils.h"

namespace
{
	using namespace mb;

	template<typename StartIt, typename EndIt>
	ohlcv_data merge_ohlcv(StartIt start, EndIt end, std::time_t targetTime)
	{
		double open = start->open();
		double high = start->high();
		double low = start->low();
		double close = start->close();
		double volume = start->volume();

		auto it = std::make_reverse_iterator(start);
		for (; it != end; ++it)
		{
			const ohlcv_data& data = *it;

			if (data.time_stamp() < targetTime)
			{
				break;
			}

			open = data.open();
			high = std::max(high, data.high());
			low = std::min(low, data.low());
			volume += data.volume();
		}

		return ohlcv_data{ targetTime, open, high, low, close, volume };
	}

	std::optional<data_iterator> get_iterator(const std::vector<ohlcv_data>& data, const tradable_pair& pair, std::time_t time, iterator_cache& cache)
	{
		if (data.empty() || time < data.front().time_stamp())
		{
			return std::nullopt;
		}

		if (time > data.back().time_stamp())
		{
			return std::prev(data.end());
		}

		auto it = find_or_default(cache, pair, data.begin());

		for (it; it != data.end(); ++it)
		{
			std::time_t timeStamp = it->time_stamp();

			if (timeStamp > time)
			{
				--it;
				break;
			}

			if (timeStamp == time)
			{
				break;
			}
		}

		cache[pair] = it;
		return it;
	}
}

namespace mb
{
	back_testing_data::back_testing_data(
		std::vector<tradable_pair> tradablePairs,
		std::unordered_map<tradable_pair, std::vector<ohlcv_data>> data,
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
		_dataSource{ std::move(dataSource) },
		_dataTime{ _startTime }, 
		_iteratorCache{}
	{}

	const std::vector<ohlcv_data>& back_testing_data::get_or_load_data(const tradable_pair& pair)
	{
		auto it = _data.find(pair);
		if (it != _data.end())
		{
			return it->second;
		}

		if (_dataSource)
		{
			std::vector<ohlcv_data> data = _dataSource->load_data(pair, _stepSize);
			return _data[pair] = std::move(data);
		}

		return _data[pair] = {};
	}

	void back_testing_data::increment()
	{
		_dataTime += _stepSize;
	}

	std::vector<ohlcv_data> back_testing_data::get_ohlcv(const tradable_pair& pair, int interval, int count)
	{
		const std::vector<ohlcv_data>& pairData{ get_or_load_data(pair) };
		std::optional<data_iterator> optStartIterator = get_iterator(pairData, pair, _dataTime, _iteratorCache);
		
		if (!optStartIterator.has_value())
		{
			return {};
		}

		data_iterator startIterator = *optStartIterator;
		std::time_t targetTime = _dataTime - interval;
		std::vector<ohlcv_data> data;
		data.reserve(count);

		for (int i = 0; i < count; i++)
		{
			ohlcv_data mergedOhlcv{ merge_ohlcv(startIterator, pairData.rend(), targetTime) };
			data.emplace(data.begin(), std::move(mergedOhlcv));

			if (startIterator == pairData.begin())
			{
				break;
			}

			while (startIterator->time_stamp() > targetTime && startIterator != pairData.begin())
			{
				startIterator--;
			}

			targetTime -= interval;
		}

		return data;
	}

	double back_testing_data::get_price(const tradable_pair& pair)
	{
		const std::vector<ohlcv_data>& pairData{ get_or_load_data(pair) };
		std::optional<data_iterator> iterator = get_iterator(pairData, pair, _dataTime, _iteratorCache);

		if (!iterator.has_value())
		{
			return 0.0;
		}

		return iterator.value()->close();
	}

	order_book_state back_testing_data::get_order_book(const tradable_pair& pair, int depth)
	{
		const std::vector<ohlcv_data>& pairData{ get_or_load_data(pair) };
		std::optional<data_iterator> iterator = get_iterator(pairData, pair, _dataTime, _iteratorCache);

		if (!iterator.has_value())
		{
			return order_book_state{ {},{} };
		}

		const ohlcv_data& ohlcvData = *iterator.value();
		return order_book_state
		{
			{
				order_book_entry{ ohlcvData.low(), ohlcvData.volume(), order_book_side::ASK }
			},
			{
				order_book_entry{ ohlcvData.high(), ohlcvData.volume(), order_book_side::BID }
			}
		};
	}
}