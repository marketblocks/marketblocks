#include <gtest/gtest.h>

#include "unittest/assertion_helpers.h"
#include "trading/moving_candle.h"

namespace mb::test
{
	TEST(MovingCandle, PushTradeAddsTradeToCandle)
	{
		moving_candle movingCandle{ 60 };

		movingCandle.push_trade(trade_update{ 10, 1.0, 2.0 });

		ohlcv_data expectedOhlcv{ 10, 1.0, 1.0, 1.0, 1.0, 2.0 };
		assert_ohlcv_data_eq(expectedOhlcv, movingCandle.get_ohlcv(10));
	}

	TEST(MovingCandle, PushingMultipleTradesCalculatesOHLCV)
	{
		moving_candle movingCandle{ 60 };

		movingCandle.push_trade(trade_update{ 10, 1.0, 2.0 });
		movingCandle.push_trade(trade_update{ 12, 3.0, 4.0 });

		ohlcv_data expectedOhlcv{ 10, 1.0, 3.0, 1.0, 3.0, 6.0 };
		assert_ohlcv_data_eq(expectedOhlcv, movingCandle.get_ohlcv(12));
	}

	TEST(MovingCandle, FirstTradeSetsCandleStartTime)
	{
		moving_candle movingCandle{ 60 };

		movingCandle.push_trade(trade_update{ 10, 1.0, 2.0 });
		movingCandle.push_trade(trade_update{ 60, 3.0, 4.0 });

		ohlcv_data expectedOhlcv{ 10, 1.0, 3.0, 1.0, 3.0, 6.0 };
		assert_ohlcv_data_eq(expectedOhlcv, movingCandle.get_ohlcv(60));
	}

	TEST(MovingCandle, PushTradeAfterCandleIntervalExpiredRemovesAppropriateTrades)
	{
		moving_candle movingCandle{ 60 };

		movingCandle.push_trade(trade_update{ 10, 1.0, 2.0 });
		movingCandle.push_trade(trade_update{ 12, 3.0, 4.0 });
		movingCandle.push_trade(trade_update{ 20, 5.0, 6.0 });
		movingCandle.push_trade(trade_update{ 75, 7.0, 8.0 });

		ohlcv_data expectedOhlcv{ 15, 3.0, 7.0, 3.0, 7.0, 14.0 };
		assert_ohlcv_data_eq(expectedOhlcv, movingCandle.get_ohlcv(75));
	}
}