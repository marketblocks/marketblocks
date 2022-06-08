#pragma once

#include "back_testing_data.h"
#include "exchanges/websockets/websocket_stream.h"

namespace mb
{
	class backtest_websocket_stream : public websocket_stream
	{
	private:
		std::shared_ptr<back_testing_data_navigator> _dataNavigator;
		std::unordered_set<tradable_pair> _subscribedPairs;
		set_queue<tradable_pair> _messageQueue;

	public:
		backtest_websocket_stream(std::shared_ptr<back_testing_data_navigator> dataSource);

		void notify_data_incremented();

		void reset() override {}
		void disconnect()  override {}
		ws_connection_status connection_status() const override { return ws_connection_status::OPEN; }

		void subscribe(const websocket_subscription& subscription) override;
		void unsubscribe(const websocket_subscription& subscription) override;
		bool is_subscribed(const websocket_subscription& subscription) override;

		order_book_state get_order_book(const tradable_pair& pair, order_book_depth depth) const override;
		double get_price(const tradable_pair& pair) const override;
		ohlcv_data get_last_candle(const tradable_pair& pair, ohlcv_interval interval) const override;

		set_queue<tradable_pair>& get_order_book_message_queue() override
		{
			return _messageQueue;
		}
	};
}