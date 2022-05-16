#include <gtest/gtest.h>

#include "testing/back_testing/back_testing_data.h"

namespace mb::test
{
	TEST(BackTestingData, GetIndexReturnsNotFoundIfTimeIsBeforeFirstDataPoint)
	{
		std::vector<timed_ohlcv_data> data
		{
			timed_ohlcv_data{ 2, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 3, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 4, ohlcv_data{ 0, 0, 0, 0, 0 } },
		};

		EXPECT_EQ(-1, internal::get_data_index(data, 1));
		EXPECT_EQ(-1, internal::get_data_index(data, 0));
		EXPECT_EQ(-1, internal::get_data_index(data, -1));
	}

	TEST(BackTestingData, GetIndexReturnsCorrectlyIfTimeIsEqualToDataPoint)
	{
		std::vector<timed_ohlcv_data> data
		{
			timed_ohlcv_data{ 2, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 3, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 4, ohlcv_data{ 0, 0, 0, 0, 0 } },
		};

		EXPECT_EQ(0, internal::get_data_index(data, 2));
		EXPECT_EQ(1, internal::get_data_index(data, 3));
		EXPECT_EQ(2, internal::get_data_index(data, 4));
	}

	TEST(BackTestingData, GetIndexReturnsLastIndexIfTimeIsPastEndDataPoint)
	{
		std::vector<timed_ohlcv_data> data
		{
			timed_ohlcv_data{ 2, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 3, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 4, ohlcv_data{ 0, 0, 0, 0, 0 } },
		};

		EXPECT_EQ(2, internal::get_data_index(data, 5));
		EXPECT_EQ(2, internal::get_data_index(data, 10));
	}

	TEST(BackTestingData, GetIndexReturnsMostRecentIndexIfTimeIsBetweenDataPoints)
	{
		std::vector<timed_ohlcv_data> data
		{
			timed_ohlcv_data{ 2, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 5, ohlcv_data{ 0, 0, 0, 0, 0 } },
			timed_ohlcv_data{ 7, ohlcv_data{ 0, 0, 0, 0, 0 } },
		};

		EXPECT_EQ(0, internal::get_data_index(data, 4));
		EXPECT_EQ(1, internal::get_data_index(data, 6));
	}
}