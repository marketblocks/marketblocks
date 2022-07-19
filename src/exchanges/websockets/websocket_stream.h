#pragma once

#include "websocket_subscription.h"
#include "networking/websocket/websocket_connection.h"
#include "trading/tradable_pair.h"
#include "trading/order_book.h"
#include "trading/ohlcv_data.h"
#include "trading/trade_update.h"
#include "common/types/set_queue.h"

namespace mb
{
	class websocket_stream
	{
	public:
		virtual ~websocket_stream() = default;

		virtual void reset() = 0;
		virtual void disconnect() = 0;
		virtual ws_connection_status connection_status() const = 0;

		virtual void subscribe(const websocket_subscription& subscription) = 0;
		virtual void unsubscribe(const websocket_subscription& subscription) = 0;
		virtual subscription_status get_subscription_status(const unique_websocket_subscription& subscription) const = 0;

		virtual order_book_state get_order_book(const tradable_pair& pair, int depth = 0) const = 0;
		virtual trade_update get_last_trade(const tradable_pair& pair) const = 0;
		virtual ohlcv_data get_last_candle(const tradable_pair& pair, ohlcv_interval interval) const = 0;

		virtual set_queue<tradable_pair>& get_order_book_message_queue() = 0;
	};
}