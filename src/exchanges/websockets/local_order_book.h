#pragma once

#include <string_view>
#include <vector>
#include <functional>
#include <mutex>

#include "order_book_cache.h"
#include "common/types/unordered_string_map.h"
#include "trading/tradable_pair.h"

namespace cb
{
	class local_order_book
	{
	private:
		std::function<void(tradable_pair)> _onMessage;
		unordered_string_map<order_book_cache> _orderBookCaches;
		mutable std::mutex _mutex;

	public:
		local_order_book();
		local_order_book(const local_order_book& other);
		local_order_book(local_order_book&& other) noexcept;

		local_order_book& operator=(const local_order_book& other);
		local_order_book& operator=(local_order_book&& other) noexcept;

		void set_message_handler(std::function<void(tradable_pair)> onMessage) noexcept { _onMessage = onMessage; }
		order_book_state get_order_book(const tradable_pair& pair, int depth = 0) const;

		bool is_subscribed(const tradable_pair& pair) const;
		void initialise_book(tradable_pair pair, ask_map asks, bid_map bids, int depth);
		void update_book(const tradable_pair& pair, order_book_cache_entry cacheEntry);
		void replace_in_book(const tradable_pair& pair, std::string_view oldPrice, order_book_cache_entry cacheEntry);
	};
}