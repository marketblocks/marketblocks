#include <utility>

#include "order_book.h"

namespace
{
	std::vector<cb::order_book_level> convert_to_levels(const std::vector<cb::order_book_entry>& askEntries, const std::vector<cb::order_book_entry>& bidEntries)
	{
		std::vector<cb::order_book_level> levels;
		levels.reserve(askEntries.size());

		for (int i = 0; i < askEntries.size(); ++i)
		{
			levels.emplace_back(askEntries[i], bidEntries[i]);
		}

		return levels;
	}
}

namespace cb
{
	order_book_entry::order_book_entry(order_book_side side, double price, double volume, double timeStamp)
		: _side{ side }, _price{ price }, _volume{ volume }, _timeStamp{ timeStamp }
	{}

	order_book_level::order_book_level(order_book_entry askEntry, order_book_entry bidEntry)
		: _askEntry{ std::move(askEntry) }, _bidEntry{ std::move(bidEntry) }
	{}

	order_book_state::order_book_state(std::vector<order_book_level> levels)
		: _levels{ std::move(levels) }
	{}

	order_book_state::order_book_state(std::vector<order_book_entry> askEntries, std::vector<order_book_entry> bidEntries)
		: _levels{ convert_to_levels(askEntries, bidEntries) }
	{
	}

	int order_book_state::depth() const
	{
		return _levels.size();
	}

	const order_book_level& order_book_state::level(int i) const
	{
		return _levels.at(i);
	}
}