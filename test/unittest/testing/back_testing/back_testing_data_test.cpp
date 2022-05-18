#include <gtest/gtest.h>

#include "testing/back_testing/back_testing_data.h"

namespace mb::test
{
	TEST(BackTestingData, FindDataPositionReturnsEndIfTimeIsBeforeFirstDataPoint)
	{
		std::vector<timed_ohlcv_data> data
		{
			timed_ohlcv_data{ 2, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 3, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 4, ohlcv_data{ 0, 0, 0, 0, 0 } },
		};

		tradable_pair pair{ "BTC", "GBP" };

		back_testing_data_navigator navigator
		{
			back_testing_data
			{
				std::vector<tradable_pair>{ pair },
				std::unordered_map<tradable_pair,std::vector<timed_ohlcv_data>>{ { pair, data }},
				1,
				4,
				1,
				3
			}
		};

		auto iteratedData = navigator.data().get_ohlcv_data(pair);

		EXPECT_EQ(iteratedData.end(), navigator.find_data_position(iteratedData, pair));
	}

	TEST(BackTestingData, FindDataPositionReturnsCorrectlyIfTimeIsEqualToDataPoint)
	{
		std::vector<timed_ohlcv_data> data
		{
			timed_ohlcv_data{ 2, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 3, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 4, ohlcv_data{ 0, 0, 0, 0, 0 } },
		};

		tradable_pair pair{ "BTC", "GBP" };

		back_testing_data_navigator navigator
		{
			back_testing_data
			{
				std::vector<tradable_pair>{ pair },
				std::unordered_map<tradable_pair,std::vector<timed_ohlcv_data>>{ { pair, data }},
				1,
				4,
				1,
				3
			}
		};

		auto iteratedData = navigator.data().get_ohlcv_data(pair);

		navigator.increment_data();
		EXPECT_EQ(iteratedData.begin(), navigator.find_data_position(iteratedData, pair));

		navigator.increment_data();
		EXPECT_EQ(iteratedData.begin() + 1, navigator.find_data_position(iteratedData, pair));

		navigator.increment_data();
		EXPECT_EQ(iteratedData.begin() + 2, navigator.find_data_position(iteratedData, pair));
	}

	TEST(BackTestingData, FindDataPositionReturnsLastIfTimeIsPastEndDataPoint)
	{
		std::vector<timed_ohlcv_data> data
		{
			timed_ohlcv_data{ 2, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 3, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 4, ohlcv_data{ 0, 0, 0, 0, 0 } },
		};

		tradable_pair pair{ "BTC", "GBP" };

		back_testing_data_navigator navigator
		{
			back_testing_data
			{
				std::vector<tradable_pair>{ pair },
				std::unordered_map<tradable_pair,std::vector<timed_ohlcv_data>>{ { pair, data }},
				5,
				6,
				1,
				5
			}
		};

		auto iteratedData = navigator.data().get_ohlcv_data(pair);

		EXPECT_EQ(iteratedData.end() - 1, navigator.find_data_position(iteratedData, pair));

		navigator.increment_data();
		EXPECT_EQ(iteratedData.end() - 1, navigator.find_data_position(iteratedData, pair));
	}

	TEST(BackTestingData, FindDataPositionReturnsMostRecentIfTimeIsBetweenDataPoints)
	{
		std::vector<timed_ohlcv_data> data
		{
			timed_ohlcv_data{ 2, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 5, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 7, ohlcv_data{ 0, 0, 0, 0, 0 } },
		};

		tradable_pair pair{ "BTC", "GBP" };

		back_testing_data_navigator navigator
		{
			back_testing_data
			{
				std::vector<tradable_pair>{ pair },
				std::unordered_map<tradable_pair,std::vector<timed_ohlcv_data>>{ { pair, data }},
				2,
				7,
				1,
				3
			}
		};

		auto iteratedData = navigator.data().get_ohlcv_data(pair);

		navigator.increment_data();
		navigator.increment_data();
		EXPECT_EQ(iteratedData.begin(), navigator.find_data_position(iteratedData, pair));

		navigator.increment_data();
		navigator.increment_data();
		EXPECT_EQ(iteratedData.begin() + 1, navigator.find_data_position(iteratedData, pair));
	}
}