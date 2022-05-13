#include "backtest_websocket_stream.h"
#include "trading/ohlcv_data.h"

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

	void backtest_websocket_stream::subscribe_order_book(const std::vector<tradable_pair>& tradablePairs)
	{
		_subscribedPairs.insert(tradablePairs.begin(), tradablePairs.end());
	}

	void backtest_websocket_stream::unsubscribe_order_book(const std::vector<tradable_pair>& tradablePairs)
	{
		for (auto& pair : tradablePairs)
		{
			_subscribedPairs.erase(pair);
		}
	}

	order_book_state backtest_websocket_stream::get_order_book(const tradable_pair& pair, int depth) const
	{
		std::optional<std::reference_wrapper<const timed_ohlcv_data>> data{ internal::get_data(*_dataNavigator, pair) };

		if (data.has_value())
		{
			const ohlcv_data& ohlcvData = data.value().get().data();
			return order_book_state
			{
				{
					order_book_entry{ ohlcvData.low(), ohlcvData.volume() }
				},
				{
					order_book_entry{ ohlcvData.high(), ohlcvData.volume() }
				}
			};
		}

		return order_book_state{ {},{} };
	}

	set_queue<tradable_pair>& backtest_websocket_stream::get_order_book_message_queue()
	{
		return _messageQueue;
	}
}