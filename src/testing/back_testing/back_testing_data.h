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
		int _stepSize;
		int _timeSteps;

	public:
		back_testing_data(
			std::vector<tradable_pair> tradablePairs,
			std::unordered_map<tradable_pair, std::vector<timed_ohlcv_data>> data,
			std::time_t startTime,
			int stepSize,
			int timeSteps);

		std::time_t start_time() const noexcept { return _startTime; }
		int step_size() const noexcept { return _stepSize; }
		int time_steps() const noexcept { return _timeSteps; }
		const std::vector<tradable_pair>& tradable_pairs() const noexcept { return _tradablePairs; }
		const std::vector<timed_ohlcv_data>& get_ohlcv_data(const tradable_pair& pair) const;
	};

	class back_testing_data_navigator
	{
	private:
		back_testing_data _data;
		std::time_t _dataTime;

	public:
		back_testing_data_navigator(back_testing_data data);

		void increment_data() { _dataTime += _data.step_size(); };
		const back_testing_data& data() const noexcept { return _data; }
		std::time_t data_time() const noexcept { return _dataTime; }
	};

	namespace internal
	{
		int get_data_index(const std::vector<timed_ohlcv_data>& data, std::time_t time);
		std::optional<std::reference_wrapper<const timed_ohlcv_data>> get_data(const back_testing_data_navigator& dataNavigator, const tradable_pair& tradablePair);
	}
}