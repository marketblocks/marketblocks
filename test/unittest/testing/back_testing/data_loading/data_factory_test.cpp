#include <gtest/gtest.h>

#include "testing/back_testing/data_loading/data_factory.h"
#include "test_data/test_data_constants.h"
#include "unittest/mocks.h"
#include "unittest/assertion_helpers.h"

namespace mb::test
{
	using ::testing::_;
	using ::testing::Return;

	static std::string TestDataDirectory = std::string{ TEST_DATA_FOLDER } + "/" + "csv_data_source_test";

	TEST(DataFactory, UsesConfigTimeLimitsIfDynamicLoadingEnabled)
	{
		constexpr std::time_t startTime = 10;
		constexpr std::time_t endTime = 50;
		constexpr int stepSize = 10;
		constexpr int expectedTimeSteps = 5;

		back_testing_config config
		{
			startTime,
			endTime,
			stepSize,
			TestDataDirectory,
			true
		};

		std::unique_ptr<back_testing_data_source> mockDataSource{ std::make_unique<mock_back_testing_data_source>() };
		std::shared_ptr<back_testing_data> data{ load_back_testing_data(std::move(mockDataSource), config) };

		EXPECT_EQ(startTime, data->start_time());
		EXPECT_EQ(endTime, data->end_time());
		EXPECT_EQ(expectedTimeSteps, data->time_steps());
	}

	TEST(DataFactory, DoesNotLoadDataIfDynamicLoadingEnabled)
	{
		back_testing_config config
		{
			10,
			50,
			10,
			TestDataDirectory,
			true
		};

		std::vector<tradable_pair> expectedPairs
		{
			tradable_pair{ "BTC", "USD" },
			tradable_pair{ "ETH", "GBP" }
		};

		std::unique_ptr<mock_back_testing_data_source> mockDataSource{ std::make_unique<mock_back_testing_data_source>() };

		EXPECT_CALL(*mockDataSource, load_data(_, _)).Times(0);
		EXPECT_CALL(*mockDataSource, get_available_pairs()).WillOnce(Return(expectedPairs));

		std::shared_ptr<back_testing_data> data{ load_back_testing_data(std::move(mockDataSource), config) };
		
		EXPECT_EQ(expectedPairs, data->tradable_pairs());
	}

	TEST(DataFactory, DataLoadedIfDynamicLoadingDisabled)
	{
		back_testing_config config
		{
			100,
			340,
			60,
			TestDataDirectory,
			false
		};

		std::vector<tradable_pair> expectedPairs
		{
			tradable_pair{ "BTC", "USD" }
		};

		std::unique_ptr<mock_back_testing_data_source> mockDataSource{ std::make_unique<mock_back_testing_data_source>() };

		EXPECT_CALL(*mockDataSource, load_data(expectedPairs[0], _)).Times(1);
		EXPECT_CALL(*mockDataSource, get_available_pairs()).WillOnce(Return(expectedPairs));

		std::shared_ptr<back_testing_data> data{ load_back_testing_data(std::move(mockDataSource), config) };
	}

	TEST(DataFactory, DataLimitsUsedWhenConfigLimitsZero)
	{
		back_testing_config config
		{
			0,
			0,
			60,
			TestDataDirectory,
			false
		};

		tradable_pair pair{ "BTC", "USD" };

		std::vector<tradable_pair> expectedPairs
		{
			pair
		};

		std::vector<ohlcv_data> expectedData
		{
			ohlcv_data{ 100, 1, 2, 3, 4, 5 },
			ohlcv_data{ 160, 6, 7, 8, 9, 10 },
			ohlcv_data{ 220, 11, 12, 13, 14, 15 },
			ohlcv_data{ 280, 16, 17, 18, 19, 20 },
			ohlcv_data{ 340, 21, 22, 23, 24, 25 }
		};

		std::unique_ptr<mock_back_testing_data_source> mockDataSource{ std::make_unique<mock_back_testing_data_source>() };

		EXPECT_CALL(*mockDataSource, load_data(pair, _)).WillOnce(Return(expectedData));
		EXPECT_CALL(*mockDataSource, get_available_pairs()).WillOnce(Return(expectedPairs));

		std::shared_ptr<back_testing_data> data{ load_back_testing_data(std::move(mockDataSource), config) };

		EXPECT_EQ(100, data->start_time());
		EXPECT_EQ(340, data->end_time());
		EXPECT_EQ(5, data->time_steps());
	}

	TEST(DataFactory, ConfigLimitsUsedWhenSpecified)
	{
		constexpr std::time_t startTime = 50;
		constexpr std::time_t endTime = 400;

		back_testing_config config
		{
			startTime,
			endTime,
			60,
			TestDataDirectory,
			false
		};

		tradable_pair pair{ "BTC", "USD" };

		std::vector<tradable_pair> expectedPairs
		{
			pair
		};

		std::vector<ohlcv_data> expectedData
		{
			ohlcv_data{ 100, 1, 2, 3, 4, 5 },
			ohlcv_data{ 160, 6, 7, 8, 9, 10 },
			ohlcv_data{ 220, 11, 12, 13, 14, 15 },
			ohlcv_data{ 280, 16, 17, 18, 19, 20 },
			ohlcv_data{ 340, 21, 22, 23, 24, 25 }
		};

		std::unique_ptr<mock_back_testing_data_source> mockDataSource{ std::make_unique<mock_back_testing_data_source>() };

		EXPECT_CALL(*mockDataSource, load_data(pair, _)).WillOnce(Return(expectedData));
		EXPECT_CALL(*mockDataSource, get_available_pairs()).WillOnce(Return(expectedPairs));

		std::shared_ptr<back_testing_data> data{ load_back_testing_data(std::move(mockDataSource), config) };

		EXPECT_EQ(startTime, data->start_time());
		EXPECT_EQ(endTime, data->end_time());
		EXPECT_EQ(6, data->time_steps());
	}
}