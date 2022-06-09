#include "backtest_websocket_stream.h"
#include "trading/ohlcv_data.h"

#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	backtest_websocket_stream::backtest_websocket_stream(std::shared_ptr<back_testing_data_navigator> dataSource)
		: _dataNavigator{ std::move(dataSource) }
	{}

	void backtest_websocket_stream::notify_data_incremented()
	{
		for (auto& pair : _subscribedPairs)
		{
			_messageQueue.push(tradable_pair{ pair });
		}
	}

	void backtest_websocket_stream::subscribe(const websocket_subscription& subscription)
	{

	}

	void backtest_websocket_stream::unsubscribe(const websocket_subscription& subscription)
	{

	}

	bool backtest_websocket_stream::is_subscribed(const websocket_subscription& subscription)
	{
		return true;
	}

	order_book_state backtest_websocket_stream::get_order_book(const tradable_pair& pair, int depth) const
	{
		std::optional<std::reference_wrapper<const timed_ohlcv_data>> data = _dataNavigator->find_data_point(pair);

		if (data.has_value())
		{
			const ohlcv_data& ohlcvData = data.value().get().data();
			return order_book_state
			{
				{
					order_book_entry{ ohlcvData.low(), ohlcvData.volume(), order_book_side::ASK }
				},
				{
					order_book_entry{ ohlcvData.high(), ohlcvData.volume(), order_book_side::BID }
				}
			};
		}

		return order_book_state{ {},{} };
	}

	double backtest_websocket_stream::get_price(const tradable_pair& pair) const
	{
		std::optional<std::reference_wrapper<const timed_ohlcv_data>> data = _dataNavigator->find_data_point(pair);

		if (data.has_value())
		{
			return data.value().get().data().close();
		}

		return 0.0;
	}

	ohlcv_data backtest_websocket_stream::get_last_candle(const tradable_pair& pair, ohlcv_interval interval) const
	{
		std::optional<std::reference_wrapper<const timed_ohlcv_data>> data = _dataNavigator->find_data_point(pair);

		if (data.has_value())
		{
			return data.value().get().data();
		}

		return ohlcv_data{ 0,0,0,0,0 };
	}
}