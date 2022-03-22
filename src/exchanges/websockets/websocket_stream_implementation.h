#pragma once

#include "exchanges/websockets/local_order_book.h"
#include "exchanges/exchange_status.h"
#include "trading/tradable_pair.h"

namespace cb::internal
{
	class websocket_stream_implementation
	{
	protected:
		local_order_book _localOrderBook;

	public:
		virtual std::string stream_url() const noexcept = 0;

		virtual void on_open() = 0;
		virtual void on_close(std::string_view reason) = 0;
		virtual void on_fail(std::string_view reason) = 0;
		virtual void on_message(std::string_view message) = 0;

		virtual std::string get_order_book_subscription_message(const std::vector<tradable_pair>& tradablePairs) const = 0;
		order_book_state get_order_book(const tradable_pair& pair, int depth = 0) const { return _localOrderBook.get_order_book(pair, depth); }
		set_queue<tradable_pair>& get_order_book_message_queue() noexcept { return _localOrderBook.message_queue(); }
	};
}
