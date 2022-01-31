#pragma once

#include <string>
#include <map>
#include <mutex>

#include "trading/order_book.h"
#include "common/utils/stringutils.h"

namespace cb
{
	class order_book_cache
	{
	public:
		int _depth;
		std::map<std::string, order_book_entry, numeric_string_less> _asks;
		std::map<std::string, order_book_entry, numeric_string_greater> _bids;
		mutable std::mutex _mutex;

	public:
		order_book_cache(int depth);

		order_book_cache(const order_book_cache&);
		order_book_cache(order_book_cache&&) noexcept;

		order_book_cache& operator=(const order_book_cache&);
		order_book_cache& operator=(order_book_cache&&) noexcept;

		void cache(std::string price, order_book_entry entry);
		void replace(const std::string& oldPrice, std::string newPrice, order_book_entry newEntry);

		order_book_state snapshot() const;
	};
}