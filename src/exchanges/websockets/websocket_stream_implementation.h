#pragma once

#include "exchanges/websockets/local_order_book.h"
#include "exchanges/exchange_status.h"
#include "trading/tradable_pair.h"
#include "trading/ohlcv_data.h"

namespace mb::internal
{
	class websocket_stream_implementation
	{
	protected:
		local_order_book _localOrderBook;

	public:
		virtual ~websocket_stream_implementation() = default;

		virtual std::string stream_url() const noexcept = 0;

		virtual void on_open() = 0;
		virtual void on_close(std::error_code reason) = 0;
		virtual void on_fail(std::error_code reason) = 0;
		virtual void on_message(std::string_view message) = 0;

		virtual std::string get_order_book_subscription_message(const std::vector<tradable_pair>& tradablePairs) const = 0;
		virtual std::string get_order_book_unsubscription_message(const std::vector<tradable_pair>& tradablePairs) const = 0;
		bool is_order_book_subscribed(const tradable_pair& pair) const { return _localOrderBook.is_subscribed(pair); }
		order_book_state get_order_book(const tradable_pair& pair, int depth = 0) const { return _localOrderBook.get_order_book(pair, depth); }
		set_queue<tradable_pair>& get_order_book_message_queue() noexcept { return _localOrderBook.message_queue(); }

		virtual std::string get_price_subscription_message(const std::vector<tradable_pair>& tradablePairs) const = 0;
		virtual std::string get_price_unsubscription_message(const std::vector<tradable_pair>& tradablePairs) const = 0;
		virtual bool is_price_subscribed(const tradable_pair& pair) const = 0;
		virtual double get_price(const tradable_pair& pair) const = 0;

		virtual std::string get_candles_subscription_message(const std::vector<tradable_pair>& tradablePairs, int interval) const = 0;
		virtual std::string get_candles_unsubscription_message(const std::vector<tradable_pair>& tradablePairs, int interval) const = 0;
		virtual bool is_candles_subscribed(const tradable_pair& pair, int interval) const = 0;
		virtual ohlcv_data get_candle(const tradable_pair& pair, int interval) const = 0;
	};
}
