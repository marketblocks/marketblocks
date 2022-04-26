#include "backtest_websocket_stream.h"

namespace mb
{
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
		return order_book_state{ {},{} };
	}

	set_queue<tradable_pair>& backtest_websocket_stream::get_order_book_message_queue()
	{
		return _messageQueue;
	}
}