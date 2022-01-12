#include "order_book_cache.h"
#include "common/utils/containerutils.h"

OrderBookCache::OrderBookCache(int depth)
	: _asks{ depth }, _bids{ depth }, _mutex{}, count{ 0 }
{}

OrderBookCache::OrderBookCache(const OrderBookCache& other)
	: _asks{ other._asks }, _bids{ other._bids }, _mutex{}, count{ 0 }
{}

OrderBookCache::OrderBookCache(OrderBookCache&& other)
	: _asks{ std::move(other._asks) }, _bids{ std::move(other._bids) }, count{ 0 }
{
}

OrderBookCache& OrderBookCache::operator=(const OrderBookCache& other)
{
	_asks = other._asks;
	_bids = other._bids;

	return *this;
}

OrderBookCache& OrderBookCache::operator=(OrderBookCache&& other)
{
	_asks = std::move(other._asks);
	_bids = std::move(other._bids);

	return *this;
}

void OrderBookCache::cache(std::string price, OrderBookEntry entry)
{
	std::lock_guard<std::mutex> lock{ _mutex };

	if (entry.side() == OrderBookSide::ASK)
	{
		_asks.emplace(std::move(price), std::move(entry));
	}
	else
	{
		_bids.emplace(std::move(price), std::move(entry));
	}

	count++;
}

OrderBookState OrderBookCache::snapshot() const
{
	std::lock_guard<std::mutex> lock{ _mutex };

	return OrderBookState
	{ 
		to_vector<OrderBookEntry>(_asks, [](const std::pair<std::string, OrderBookEntry>& pair) { return pair.second; }),
		to_vector<OrderBookEntry>(_bids, [](const std::pair<std::string, OrderBookEntry>& pair) { return pair.second; })
	};
}