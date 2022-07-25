#include "order_book.h"

namespace mb
{
	order_book_state::order_book_state(
		std::time_t timeStamp,
		std::vector<order_book_entry> asks,
		std::vector<order_book_entry> bids)
		: _timeStamp{ timeStamp }, _asks { std::move(asks) }, _bids{ std::move(bids) }
	{
	}

	const order_book_entry& get_best_entry(const std::vector<order_book_entry>& entries)
	{
		assert(!entries.empty());
		return entries[0];
	}

	const order_book_entry& select_best_entry(const order_book_state& orderBook, trade_action action)
	{
		return action == trade_action::BUY
			? get_best_entry(orderBook.asks())
			: get_best_entry(orderBook.bids());
	}
}