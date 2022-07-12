#include <gtest/gtest.h>

#include "trading/ohlcv_from_trades.h"
#include "unittest/assertion_helpers.h"

namespace mb::test
{
	static ohlcv_data OHLCV_DATA = ohlcv_data
	{
		100, 2, 4, 1, 3, 0.1
	};

	TEST(OhlcvFromTrades, GetOhlcvReturnsCurrentSetValues)
	{
		ohlcv_from_trades oft{ OHLCV_DATA, 60 };
		assert_ohlcv_data_eq(OHLCV_DATA, oft.get_ohlcv(100));
	}

	TEST(OhlcvFromTrades, GetOhlcvAfterCandleExpiredReturnsNewOhlcvData)
	{
		ohlcv_from_trades oft{ OHLCV_DATA, 60 };

		ohlcv_data expectedOhlcv{ 160, 3, 3, 3, 3, 0 };

		assert_ohlcv_data_eq(expectedOhlcv, oft.get_ohlcv(200));
	}

	TEST(OhlcvFromTrades, GetOhlcvReturnsDataWithTimeOfLatestCandle)
	{
		ohlcv_from_trades oft{ OHLCV_DATA, 60 };
		EXPECT_EQ(280, oft.get_ohlcv(300).time_stamp());
	}

	TEST(OhlcvFromTrades, AddTradeOverwritesHigh)
	{
		constexpr double newHigh = 5.0;

		ohlcv_from_trades oft{ OHLCV_DATA, 60 };
		oft.add_trade(120, newHigh, 0.01);

		EXPECT_DOUBLE_EQ(newHigh, oft.get_ohlcv(120).high());
	}

	TEST(OhlcvFromTrades, AddTradeOverwritesLow)
	{
		constexpr double newLow = 0.2;

		ohlcv_from_trades oft{ OHLCV_DATA, 60 };
		oft.add_trade(120, newLow, 0.01);

		EXPECT_DOUBLE_EQ(newLow, oft.get_ohlcv(120).low());
	}

	TEST(OhlcvFromTrades, AddTradeIncreaseVolume)
	{
		constexpr double volume = 0.5;

		ohlcv_from_trades oft{ OHLCV_DATA, 60 };
		oft.add_trade(120, 4, volume);

		EXPECT_DOUBLE_EQ(OHLCV_DATA.volume() + volume, oft.get_ohlcv(120).volume());
	}

	TEST(OhlcvFromTrades, AddTradeAfterLastSetsClose)
	{
		constexpr double close = 2.0;

		ohlcv_from_trades oft{ OHLCV_DATA, 60 };
		oft.add_trade(120, 4, 0.1);
		oft.add_trade(130, close, 0.02);

		EXPECT_DOUBLE_EQ(close, oft.get_ohlcv(130).close());
	}

	TEST(OhlcvFromTrades, AddTradeBeforeStartTimeIsIgnored)
	{
		ohlcv_from_trades oft{ OHLCV_DATA, 60 };
		oft.add_trade(50, 4, 0.1);

		assert_ohlcv_data_eq(OHLCV_DATA, oft.get_ohlcv(100));
	}

	TEST(OhlcvFromTrades, AddTradeAfterIntervalCreatesNewCandle)
	{
		ohlcv_from_trades oft{ OHLCV_DATA, 60 };
		oft.add_trade(300, 4, 0.1);

		ohlcv_data expectedOhlcv{ 280, 3, 4, 3, 4, 0.1 };

		assert_ohlcv_data_eq(expectedOhlcv, oft.get_ohlcv(300));
	}
}