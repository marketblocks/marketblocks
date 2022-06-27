#pragma once

#include <vector>
#include <unordered_map>

#include "back_testing_data_source.h"
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
		std::time_t _endTime;
		int _stepSize;
		int _timeSteps;

		std::unique_ptr<back_testing_data_source> _dataSource;

	public:
		back_testing_data(
			std::vector<tradable_pair> tradablePairs,
			std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>> data,
			std::time_t startTime,
			std::time_t endTime,
			int stepSize,
			int timeSteps,
			std::unique_ptr<back_testing_data_source> dataSource = nullptr);

		std::time_t start_time() const noexcept { return _startTime; }
		std::time_t end_time() const noexcept { return _endTime; }
		int step_size() const noexcept { return _stepSize; }
		int time_steps() const noexcept { return _timeSteps; }
		const std::vector<tradable_pair>& tradable_pairs() const noexcept { return _tradablePairs; }
		const std::vector<timed_ohlcv_data>& get_ohlcv_data(const tradable_pair& pair);
	};

	class back_testing_data_navigator
	{
	private:
		back_testing_data _data;
		std::time_t _dataTime;
		std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>::const_iterator> _iteratorCache;

	public:
		back_testing_data_navigator(back_testing_data data);

		void increment_data() { _dataTime += _data.step_size(); };
		back_testing_data& data() noexcept { return _data; }
		std::time_t data_time() const noexcept { return _dataTime; }

		timed_ohlcv_data get_merged_ohlcv(const tradable_pair& pair, int interval);
		std::vector<timed_ohlcv_data> get_past_ohlcv_data(const tradable_pair& pair, int interval, int count);

		std::vector<timed_ohlcv_data>::const_iterator find_data_position(const std::vector<timed_ohlcv_data>& pairData, const tradable_pair& tradablePair);
		std::optional<std::reference_wrapper<const timed_ohlcv_data>> find_data_point(const tradable_pair& tradablePair);
	};
}