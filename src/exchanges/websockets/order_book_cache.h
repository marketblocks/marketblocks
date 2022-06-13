
#pragma once

#include <mutex>
#include <set>

#include "trading/order_book.h"
#include "common/utils/stringutils.h"

namespace mb
{
	namespace internal
	{
		struct entry_less_than
		{
			bool operator()(const order_book_entry& l, const order_book_entry& r) const;
		};

		struct entry_greater_than
		{
			bool operator()(const order_book_entry& l, const order_book_entry& r) const;
		};
	}
	
	using ask_cache = std::set<order_book_entry, internal::entry_less_than>;
	using bid_cache = std::set<order_book_entry, internal::entry_greater_than>;

	class order_book_cache
	{
	private:
		ask_cache _asks;
		bid_cache _bids;

	public:
		order_book_cache(ask_cache asks, bid_cache bids);

		void update_cache(order_book_entry entry);
		order_book_state snapshot(int depth = 0) const;
	};
}