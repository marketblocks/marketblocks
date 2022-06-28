#include "backtest_websocket_stream.h"
#include "trading/ohlcv_data.h"

#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	backtest_websocket_stream::backtest_websocket_stream(std::shared_ptr<back_testing_data> backTestingData)
		: _backTestingData{ std::move(backTestingData) }
	{}

	void backtest_websocket_stream::subscribe(const websocket_subscription& subscription)
	{}

	void backtest_websocket_stream::unsubscribe(const websocket_subscription& subscription)
	{}

	subscription_status backtest_websocket_stream::get_subscription_status(const unique_websocket_subscription& subscription) const
	{
		return subscription_status::SUBSCRIBED;
	}

	order_book_state backtest_websocket_stream::get_order_book(const tradable_pair& pair, int depth) const
	{
		return _backTestingData->get_order_book(pair, depth);
	}

	double backtest_websocket_stream::get_price(const tradable_pair& pair) const
	{
		return _backTestingData->get_price(pair);
	}

	ohlcv_data backtest_websocket_stream::get_last_candle(const tradable_pair& pair, ohlcv_interval interval) const
	{
		std::vector<ohlcv_data> candles = _backTestingData->get_ohlcv(pair, to_seconds(interval), 1);

		if (candles.empty())
		{
			return ohlcv_data{};
		}

		return candles.front();
	}
}