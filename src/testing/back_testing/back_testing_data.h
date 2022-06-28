#pragma once

#include <vector>
#include <unordered_map>

#include "data_loading/back_testing_data_source.h"
#include "trading/tradable_pair.h"
#include "trading/ohlcv_data.h"
#include "trading/order_book.h"

namespace mb
{
	using data_iterator = std::vector<ohlcv_data>::const_iterator;
	using iterator_cache = std::unordered_map<tradable_pair, data_iterator>;

	class back_testing_data
	{
	private:
		std::vector<tradable_pair> _tradablePairs;
		std::unordered_map<tradable_pair, std::vector<ohlcv_data>> _data;
		std::time_t _startTime;
		std::time_t _endTime;
		int _stepSize;
		int _timeSteps;

		std::unique_ptr<back_testing_data_source> _dataSource;

		std::time_t _dataTime;
		iterator_cache _iteratorCache;

		const std::vector<ohlcv_data>& get_or_load_data(const tradable_pair& pair);

	public:
		back_testing_data(
			std::vector<tradable_pair> tradablePairs,
			std::unordered_map<tradable_pair, std::vector<ohlcv_data>> data,
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

		void increment();
		std::vector<ohlcv_data> get_ohlcv(const tradable_pair& pair, int interval, int count);
		double get_price(const tradable_pair& pair);
		order_book_state get_order_book(const tradable_pair& pair, int depth = 0);
	};
}