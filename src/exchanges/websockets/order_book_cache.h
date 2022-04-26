
#pragma once

#include <string>
#include <map>
#include <mutex>

#include "trading/order_book.h"
#include "common/utils/stringutils.h"

namespace mb
{
	struct order_book_cache_entry
	{
		order_book_side side;
		std::string price;
		order_book_entry entry;
	};

	using ask_map = std::map<std::string, order_book_entry, numeric_string_less>;
	using bid_map = std::map<std::string, order_book_entry, numeric_string_greater>;

	class order_book_cache
	{
	private:
		int _depth;
		ask_map _asks;
		bid_map _bids;
		mutable std::mutex _mutex;

	public:
		order_book_cache(ask_map asks, bid_map bids, int depth);

		order_book_cache(const order_book_cache&);
		order_book_cache(order_book_cache&&) noexcept;

		order_book_cache& operator=(const order_book_cache&);
		order_book_cache& operator=(order_book_cache&&) noexcept;

		void update_cache(order_book_cache_entry cacheEntry);
		void remove(std::string_view price, order_book_side side);

		order_book_state snapshot(int depth = 0) const;
	};
}