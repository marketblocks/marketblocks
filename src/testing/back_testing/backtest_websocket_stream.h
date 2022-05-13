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

		void connect() override {}
		ws_connection_status connection_status() const override { return ws_connection_status::OPEN; }

		void subscribe_order_book(const std::vector<tradable_pair>& tradablePairs) override;
		void unsubscribe_order_book(const std::vector<tradable_pair>& tradablePairs) override;
		order_book_state get_order_book(const tradable_pair& pair, int depth = 0) const override;
		set_queue<tradable_pair>& get_order_book_message_queue() override;
	};
}