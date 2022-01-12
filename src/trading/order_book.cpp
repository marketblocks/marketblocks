#include <utility>

#include "order_book.h"

namespace
{
	std::vector<OrderBookLevel> convert_to_levels(const std::vector<OrderBookEntry>& askEntries, const std::vector<OrderBookEntry>& bidEntries)
	{
		std::vector<OrderBookLevel> levels;
		levels.reserve(askEntries.size());

		for (int i = 0; i < askEntries.size(); ++i)
		{
			levels.emplace_back(askEntries[i], bidEntries[i]);
		}

		return levels;
	}
}

OrderBookEntry::OrderBookEntry(OrderBookSide side, double price, double volume, double timeStamp)
	: _side{ side }, _price { price }, _volume{volume}, _timeStamp{ timeStamp }
{}

OrderBookLevel::OrderBookLevel(OrderBookEntry askEntry, OrderBookEntry bidEntry)
	: _askEntry { std::move(askEntry) }, _bidEntry{ std::move(bidEntry) }
{}

OrderBookState::OrderBookState(std::vector<OrderBookLevel> levels)
	: _levels{ std::move(levels) }
{}

OrderBookState::OrderBookState(std::vector<OrderBookEntry> askEntries, std::vector<OrderBookEntry> bidEntries)
	:_levels{ convert_to_levels(askEntries, bidEntries)}
{
}

int OrderBookState::depth() const
{
	return _levels.size();
}

const OrderBookLevel& OrderBookState::level(int i) const
{
	return _levels.at(i);
}