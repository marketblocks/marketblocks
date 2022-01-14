#include "order_book_cache.h"
#include "common/utils/containerutils.h"

namespace
{
	template<typename OrderBookMap>
	void ensure_depth(OrderBookMap& map, int depth)
	{
		auto end = map.end();
		while (map.size() > depth)
		{
			map.erase(--end);
		}
	}
}

OrderBookCache::OrderBookCache(int depth)
	: _depth{ depth }, _asks {}, _bids{}, _mutex{}
{}

OrderBookCache::OrderBookCache(const OrderBookCache& other)
	: _depth{ other._depth }, _asks { other._asks }, _bids{other._bids}, _mutex{}
{}

OrderBookCache::OrderBookCache(OrderBookCache&& other)
	: _depth{ std::move(other._depth) }, _asks{ std::move(other._asks) }, _bids{std::move(other._bids)}
{
}

OrderBookCache& OrderBookCache::operator=(const OrderBookCache& other)
{
	_depth = other._depth;
	_asks = other._asks;
	_bids = other._bids;

	return *this;
}

OrderBookCache& OrderBookCache::operator=(OrderBookCache&& other)
{
	_depth = std::move(other._depth);
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
		ensure_depth(_asks, _depth);
	}
	else
	{
		_bids.emplace(std::move(price), std::move(entry));
		ensure_depth(_bids, _depth);
	}
}

void OrderBookCache::replace(const std::string& oldPrice, std::string newPrice, OrderBookEntry newEntry)
{
	std::lock_guard<std::mutex> lock{ _mutex };

	if (newEntry.side() == OrderBookSide::ASK)
	{
		_asks.erase(oldPrice);
		_asks.emplace(std::move(newPrice), std::move(newEntry));
	}
	else
	{
		_bids.erase(oldPrice);
		_bids.emplace(std::move(newPrice), std::move(newEntry));
	}
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