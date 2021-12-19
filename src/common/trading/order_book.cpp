#include <utility>

#include "order_book.h"

OrderBookEntry::OrderBookEntry(double price, double volume)
	: _price{ price }, _volume{ volume }
{}

OrderBookLevel::OrderBookLevel(OrderBookEntry askEntry, OrderBookEntry bidEntry)
	: _askEntry { std::move(askEntry) }, _bidEntry{ std::move(bidEntry) }
{}

OrderBookState::OrderBookState(std::vector<OrderBookLevel> levels)
	: _levels{ std::move(levels) }
{}

int OrderBookState::depth() const
{
	return _levels.size();
}

const OrderBookLevel& OrderBookState::level(int i) const
{
	return _levels.at(i);
}