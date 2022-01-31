#pragma once

#include <string>
#include <map>
#include <mutex>

#include "trading/order_book.h"
#include "common/utils/stringutils.h"

namespace cb
{
	struct cache_replacement
	{
		const std::string& oldPrice;
		std::string newPrice;
		order_book_entry newEntry;
	};

	struct cache_entry
	{
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
		order_book_cache(std::vector<cache_entry> asks, std::vector<cache_entry> bids);

		order_book_cache(const order_book_cache&);
		order_book_cache(order_book_cache&&) noexcept;

		order_book_cache& operator=(const order_book_cache&);
		order_book_cache& operator=(order_book_cache&&) noexcept;

		void cache(cache_entry cacheEntry);
		void replace(cache_replacement cacheReplacement);

		order_book_state snapshot() const;
	};
}