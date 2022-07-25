#include <gtest/gtest.h>

#include "testing/back_testing/back_testing_data.h"
#include "unittest/assertion_helpers.h"

namespace mb::test
{
	static std::vector<ohlcv_data> TEST_DATA
	{
		ohlcv_data{ 100, 2, 5, 1, 4, 3 },
		ohlcv_data{ 160, 7, 10, 6, 8, 9 },
		ohlcv_data{ 220, 12, 15, 11, 13, 14 },
		ohlcv_data{ 280, 17, 20, 16, 18, 19 },
		ohlcv_data{ 340, 22, 25, 21, 23, 24 }
	};

	static tradable_pair TEST_PAIR{ "BTC", "GBP" };
	TEST(BackTestingData, GetTradeReturnsOpenPriceIfTimeEqualToDataPoint)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			340,
			60,
			5
		};

		trade_update trade{ backTestingData.get_trade(TEST_PAIR) };

		assert_trade_update_eq(trade_update{ 100, 2, 3 }, trade);
	}

	TEST(BackTestingData, GetPriceReturnsZeroIfStartLessThanFirstPoint)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			50,
			340,
			60,
			5
		};

		trade_update trade{ backTestingData.get_trade(TEST_PAIR) };

		assert_trade_update_eq(trade_update{ 0, 0, 0 }, trade);
	}

	TEST(BackTestingData, GetPriceReturnsLastClosePriceIfEndGreaterThanLastPoint)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			400,
			60,
			5
		};

		for (int i = 0; i < 5; ++i)
			backTestingData.increment();

		trade_update trade{ backTestingData.get_trade(TEST_PAIR) };

		assert_trade_update_eq(trade_update{ 340, 23, 24 }, trade);
	}

	TEST(BackTestingData, GetPriceReturnsMostRecentOpenIfTimeBetweenPoints)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			340,
			30,
			5
		};

		for (int i = 0; i < 3; ++i)
			backTestingData.increment();

		trade_update trade{ backTestingData.get_trade(TEST_PAIR) };

		assert_trade_update_eq(trade_update{ 160, 7, 9 }, trade);
	}

	TEST(BackTestingData, IncrementMovesDataPosition)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			340,
			60,
			5
		};

		backTestingData.increment();

		trade_update trade{ backTestingData.get_trade(TEST_PAIR) };

		EXPECT_EQ(160, trade.time_stamp());
	}

	TEST(BackTestingData, GetOrderBookReturnsEmptyStateIfStartLessThanFirstPoint)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			50,
			340,
			60,
			5
		};

		order_book_state expectedBook
		{
			0, {}, {}
		};

		order_book_state actualBook{ backTestingData.get_order_book(TEST_PAIR) };

		assert_order_book_state_eq(expectedBook, actualBook);
	}

	TEST(BackTestingData, GetOrderBookReturnsHighAskAndLowBid)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			340,
			60,
			5
		};

		order_book_state expectedBook
		{
			100,
			{ order_book_entry{ 5, 3, order_book_side::ASK } },
			{ order_book_entry{ 1, 3, order_book_side::BID } }
		};

		order_book_state actualBook{ backTestingData.get_order_book(TEST_PAIR) };

		assert_order_book_state_eq(expectedBook, actualBook);
	}

	TEST(BackTestingData, GetOhlcvReturnsCandleAtPreviousPointIfIntervalEqualToDataInterval)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			340,
			60,
			5
		};

		backTestingData.increment();
		backTestingData.increment();

		ohlcv_data expectedData{ 160, 7, 10, 6, 8, 9 };

		std::vector<ohlcv_data> actualData{ backTestingData.get_ohlcv(TEST_PAIR, 60, 1) };

		ASSERT_EQ(1, actualData.size());
		assert_ohlcv_data_eq(expectedData, actualData.front());
	}

	TEST(BackTestingData, GetOhlcvReturnsCandlesEqualToCount)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			340,
			60,
			5
		};

		backTestingData.increment();
		backTestingData.increment();

		std::vector<ohlcv_data> expectedData
		{
			ohlcv_data{ 160, 7, 10, 6, 8, 9 },
			ohlcv_data{ 100, 2, 5, 1, 4, 3 }
		};

		std::vector<ohlcv_data> actualData{ backTestingData.get_ohlcv(TEST_PAIR, 60, 2) };

		create_vector_equal_asserter<ohlcv_data>(assert_ohlcv_data_eq)(expectedData, actualData);
	}

	TEST(BackTestingData, GetOhlcvReturnsMaxCandlesIfCountGreaterThanCandleCount)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			340,
			60,
			5
		};

		backTestingData.increment();
		backTestingData.increment();

		std::vector<ohlcv_data> expectedData
		{
			ohlcv_data{ 160, 7, 10, 6, 8, 9 },
			ohlcv_data{ 100, 2, 5, 1, 4, 3 }
		};

		std::vector<ohlcv_data> actualData{ backTestingData.get_ohlcv(TEST_PAIR, 60, 3) };

		create_vector_equal_asserter<ohlcv_data>(assert_ohlcv_data_eq)(expectedData, actualData);
	}

	TEST(BackTestingData, GetOhlcvReturnsEmptyVectorIfStartTimeLessThanFirstPoint)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			50,
			340,
			60,
			5
		};

		std::vector<ohlcv_data> actualData{ backTestingData.get_ohlcv(TEST_PAIR, 60, 2) };

		EXPECT_TRUE(actualData.empty());
	}

	TEST(BackTestingData, GetOhlcvReturnsEmptyVectorIfTimeEqualToFirstPoint)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			340,
			60,
			5
		};

		std::vector<ohlcv_data> actualData{ backTestingData.get_ohlcv(TEST_PAIR, 60, 1) };

		EXPECT_TRUE(actualData.empty());
	}

	TEST(BackTestingData, GetOhlcvReturnsFirstCandleIfTimeBetweenFirstAndSecondPoint)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			340,
			30,
			5
		};

		backTestingData.increment();

		std::vector<ohlcv_data> expectedData
		{
			ohlcv_data{ 100, 2, 5, 1, 4, 3 }
		};

		std::vector<ohlcv_data> actualData{ backTestingData.get_ohlcv(TEST_PAIR, 60, 1) };

		create_vector_equal_asserter<ohlcv_data>(assert_ohlcv_data_eq)(expectedData, actualData);
	}

	TEST(BackTestingData, GetOhlcvReturnsDuplicateCandlesIfIntervalLessThanDataInterval)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			340,
			30,
			5
		};

		backTestingData.increment();

		std::vector<ohlcv_data> expectedData
		{
			ohlcv_data{ 120, 2, 5, 1, 4, 3 },
			ohlcv_data{ 110, 2, 5, 1, 4, 3 },
			ohlcv_data{ 100, 2, 5, 1, 4, 3 }
		};

		std::vector<ohlcv_data> actualData{ backTestingData.get_ohlcv(TEST_PAIR, 10, 3) };

		create_vector_equal_asserter<ohlcv_data>(assert_ohlcv_data_eq)(expectedData, actualData);
	}

	TEST(BackTestingData, GetOhlcvMergesCandlesWhenIntervalGreaterThanDataInterval)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			340,
			60,
			5
		};

		backTestingData.increment();
		backTestingData.increment();

		std::vector<ohlcv_data> expectedData
		{
			ohlcv_data{ 100, 2, 10, 1, 8, 12 }
		};

		std::vector<ohlcv_data> actualData{ backTestingData.get_ohlcv(TEST_PAIR, 120, 1) };

		create_vector_equal_asserter<ohlcv_data>(assert_ohlcv_data_eq)(expectedData, actualData);
	}

	TEST(BackTestingData, GetOhlcvMergesCandlesWhenIntervalGreaterThanDataIntervalAndTimeBetweenPoints)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			340,
			30,
			5
		};

		for (int i = 0; i < 5; ++i)
			backTestingData.increment();

		std::vector<ohlcv_data> expectedData
		{
			ohlcv_data{ 130, 2, 15, 1, 13, 26 },
			ohlcv_data{ 100, 2, 5, 1, 4, 3 }
		};

		std::vector<ohlcv_data> actualData{ backTestingData.get_ohlcv(TEST_PAIR, 120, 2) };

		create_vector_equal_asserter<ohlcv_data>(assert_ohlcv_data_eq)(expectedData, actualData);
	}

	TEST(BackTestingData, GetOhlcvReturnsLastCandleWhenTimeGreaterThanLastPoint)
	{
		back_testing_data backTestingData
		{
			std::vector<tradable_pair>{ TEST_PAIR },
			std::unordered_map<tradable_pair,std::vector<ohlcv_data>>{ { TEST_PAIR, TEST_DATA }},
			100,
			600,
			60,
			5
		};

		for (int i = 0; i < 7; ++i)
			backTestingData.increment();

		std::vector<ohlcv_data> expectedData
		{
			ohlcv_data{ 460, 22, 25, 21, 23, 24 },
			ohlcv_data{ 400, 22, 25, 21, 23, 24 },
		};

		std::vector<ohlcv_data> actualData{ backTestingData.get_ohlcv(TEST_PAIR, 60, 2) };

		create_vector_equal_asserter<ohlcv_data>(assert_ohlcv_data_eq)(expectedData, actualData);
	}
}