#pragma once

#include <string_view>
#include <vector>
#include <functional>

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

	public:
		void set_message_handler(std::function<void(tradable_pair)> onMessage) noexcept { _onMessage = onMessage; }
		order_book_state get_order_book(const tradable_pair& pair) const;

		bool is_subscribed(const tradable_pair& pair) const;
		void initialise_book(tradable_pair pair, std::vector<cache_entry> asks, std::vector<cache_entry> bids);
		void update_book(const tradable_pair& pair, cache_entry cacheEntry);
		void replace_in_book(const tradable_pair& pair, cache_replacement cacheReplacement);
	};
}