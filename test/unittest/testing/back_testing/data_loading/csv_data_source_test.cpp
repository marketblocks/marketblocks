#include <gtest/gtest.h>

#include "testing/back_testing/data_loading/csv_data_source.h"
#include "test_data/test_data_constants.h"
#include "unittest/assertion_helpers.h"

namespace
{
	using namespace mb;
	using namespace mb::test;

	csv_data_source create_data_source(std::string directory = "")
	{
		if (directory.empty())
		{
			return csv_data_source{ csv_back_test_data_directory() };
		}

		std::filesystem::path path{ TEST_DATA_FOLDER };
		path /= directory;
		return csv_data_source{ std::move(path) };
	}
}

namespace mb::test
{
	TEST(CSVDataSource, CorrectlyIdentifiesAvailablePairs)
	{
		std::vector<tradable_pair> expectedPairs
		{
			tradable_pair{ "BTC", "USD" },
			tradable_pair{ "ETH", "GBP" }
		};

		auto dataSource = create_data_source();

		std::vector<tradable_pair> actualPairs{ dataSource.get_available_pairs() };

		EXPECT_EQ(expectedPairs, actualPairs);
	}

	TEST(CSVDataSource, GetAvailablePairsThrowsExceptionIfDirectoryDoesNotExist)
	{
		auto dataSource = create_data_source("ThisIsAFakeFolder");

		EXPECT_ANY_THROW(dataSource.get_available_pairs());
	}

	TEST(CSVDataSource, AllDataLoadedWhenStepSizeIsZero)
	{
		tradable_pair pair{ "BTC", "USD" };

		std::vector<ohlcv_data> expectedData
		{
			ohlcv_data{ 100, 1, 2, 3, 4, 5 },
			ohlcv_data{ 160, 6, 7, 8, 9, 10 },
			ohlcv_data{ 220, 11, 12, 13, 14, 15 },
			ohlcv_data{ 280, 16, 17, 18, 19, 20 },
			ohlcv_data{ 340, 21, 22, 23, 24, 25 }
		};

		auto dataSource = create_data_source();

		std::vector<ohlcv_data> actualData{ dataSource.load_data(pair, 0) };

		create_vector_equal_asserter<ohlcv_data>(assert_ohlcv_data_eq)(expectedData, actualData);
	}

	TEST(CSVDataSource, AllDataLoadedWhenStepSizeIsEqualToDataStepSize)
	{
		tradable_pair pair{ "BTC", "USD" };

		std::vector<ohlcv_data> expectedData
		{
			ohlcv_data{ 100, 1, 2, 3, 4, 5 },
			ohlcv_data{ 160, 6, 7, 8, 9, 10 },
			ohlcv_data{ 220, 11, 12, 13, 14, 15 },
			ohlcv_data{ 280, 16, 17, 18, 19, 20 },
			ohlcv_data{ 340, 21, 22, 23, 24, 25 }
		};

		auto dataSource = create_data_source();

		std::vector<ohlcv_data> actualData{ dataSource.load_data(pair, 60) };

		create_vector_equal_asserter<ohlcv_data>(assert_ohlcv_data_eq)(expectedData, actualData);
	}

	TEST(CSVDataSource, DataIsFilteredWhenStepSizeIsGreaterThanDataStep)
	{
		tradable_pair pair{ "BTC", "USD" };

		std::vector<ohlcv_data> expectedData
		{
			ohlcv_data{ 100, 1, 2, 3, 4, 5 },
			ohlcv_data{ 220, 11, 12, 13, 14, 15 },
			ohlcv_data{ 340, 21, 22, 23, 24, 25 }
		};

		auto dataSource = create_data_source();

		std::vector<ohlcv_data> actualData{ dataSource.load_data(pair, 120) };

		create_vector_equal_asserter<ohlcv_data>(assert_ohlcv_data_eq)(expectedData, actualData);
	}

	TEST(CSVDataSource, LoadDataReturnsEmptyVectorIfPairDoesNotExist)
	{
		tradable_pair pair{ "BTC", "EUR" };

		auto dataSource = create_data_source();

		EXPECT_TRUE(dataSource.load_data(pair, 0).empty());
	}
}