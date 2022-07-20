#pragma once

#include "websocket_subscription.h"
#include "websocket_update_messages.h"
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
		using trade_update_handler = std::function<void(trade_update_message)>;
		using ohlcv_update_handler = std::function<void(ohlcv_update_message)>;
		using order_book_update_handler = std::function<void(order_book_update_message)>;

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

		void set_trade_update_handler(trade_update_handler handler);
		void set_ohlcv_update_handler(ohlcv_update_handler handler);
		void set_order_book_update_handler(order_book_update_handler handler);

	protected:
		void fire_trade_update(trade_update_message message);
		void fire_ohlcv_update(ohlcv_update_message message);
		void fire_order_book_update(order_book_update_message message);

	private:
		trade_update_handler _tradeUpdateHandler;
		ohlcv_update_handler _ohlcvUpdateHandler;
		order_book_update_handler _orderBookUpdateHandler;
	};
}