#pragma once

#include <vector>
#include <unordered_map>

#include "trading/tradable_pair.h"
#include "trading/ohlcv_data.h"

namespace mb
{
	class back_testing_data
	{
	private:
		std::vector<tradable_pair> _tradablePairs;
		std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>> _data;
		std::time_t _startTime;
		int _interval;
		int _size;

	public:
		back_testing_data(
			std::vector<tradable_pair> tradablePairs,
			std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>> data,
			std::time_t startTime,
			int interval,
			int size);

		std::time_t start_time() const noexcept { return _startTime; }
		int interval() const noexcept { return _interval; }
		int size() const noexcept { return _size; }
		const std::vector<tradable_pair>& tradable_pairs() const noexcept { return _tradablePairs; }
		const std::vector<timed_ohlcv_data>& get_ohlcv_data(const tradable_pair& pair) const;
	};

	class back_testing_data_source
	{
	private:
		back_testing_data _data;
		std::time_t _dataTime;

	public:
		back_testing_data_source(back_testing_data data);

		void increment_data() { _dataTime += _data.interval(); };
		const back_testing_data& data() const noexcept { return _data; }
		std::time_t data_time() const noexcept { return _dataTime; }
	};

	namespace internal
	{
		int get_data_index(const std::vector<timed_ohlcv_data>& data, std::time_t time);
		std::optional<std::reference_wrapper<const timed_ohlcv_data>> get_data(const back_testing_data_source& dataSource, const tradable_pair& tradablePair);
	}
}