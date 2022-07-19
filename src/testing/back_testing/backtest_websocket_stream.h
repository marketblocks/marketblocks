#pragma once

#include "back_testing_data.h"
#include "exchanges/websockets/websocket_stream.h"

namespace mb
{
	class backtest_websocket_stream : public websocket_stream
	{
	private:
		std::shared_ptr<back_testing_data> _backTestingData;
		set_queue<tradable_pair> _messageQueue;

	public:
		backtest_websocket_stream(std::shared_ptr<back_testing_data> backTestingData);

		void reset() override {}
		void disconnect()  override {}
		ws_connection_status connection_status() const override { return ws_connection_status::OPEN; }

		void subscribe(const websocket_subscription& subscription) override;
		void unsubscribe(const websocket_subscription& subscription) override;
		subscription_status get_subscription_status(const unique_websocket_subscription& subscription) const override;

		order_book_state get_order_book(const tradable_pair& pair, int depth = 0) const override;
		trade_update get_last_trade(const tradable_pair& pair) const override;
		ohlcv_data get_last_candle(const tradable_pair& pair, ohlcv_interval interval) const override;

		set_queue<tradable_pair>& get_order_book_message_queue() override
		{
			return _messageQueue;
		}
	};
}