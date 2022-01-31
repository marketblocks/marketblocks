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

	template<typename OrderBookMap>
	OrderBookMap create_map(std::vector<cb::cache_entry>& entries)
	{
		OrderBookMap map;

		for (auto& cacheEntry : entries)
		{
			map.emplace(std::move(cacheEntry.price), std::move(cacheEntry.entry));
		}

		return map;
	}
}

namespace cb
{
	order_book_cache::order_book_cache(std::vector<cache_entry> asks, std::vector<cache_entry> bids)
		: _depth{ static_cast<int>(asks.size()) }, _asks{ create_map<cb::ask_map>(asks) }, _bids{ create_map<cb::bid_map>(bids) }, _mutex{}
	{}

	order_book_cache::order_book_cache(const order_book_cache& other)
		: _depth{ other._depth }, _asks{ other._asks }, _bids{ other._bids }, _mutex{}
	{}

	order_book_cache::order_book_cache(order_book_cache&& other) noexcept
		: _depth{ std::move(other._depth) }, _asks{ std::move(other._asks) }, _bids{ std::move(other._bids) }
	{
	}

	order_book_cache& order_book_cache::operator=(const order_book_cache& other)
	{
		_depth = other._depth;
		_asks = other._asks;
		_bids = other._bids;

		return *this;
	}

	order_book_cache& order_book_cache::operator=(order_book_cache&& other) noexcept
	{
		_depth = std::move(other._depth);
		_asks = std::move(other._asks);
		_bids = std::move(other._bids);

		return *this;
	}

	void order_book_cache::cache(cache_entry cacheEntry)
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		if (cacheEntry.entry.side() == order_book_side::ASK)
		{
			_asks.emplace(std::move(cacheEntry.price), std::move(cacheEntry.entry));
			ensure_depth(_asks, _depth);
		}
		else
		{
			_bids.emplace(std::move(cacheEntry.price), std::move(cacheEntry.entry));
			ensure_depth(_bids, _depth);
		}
	}

	void order_book_cache::replace(cache_replacement cacheReplacement)
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		if (cacheReplacement.newEntry.side() == order_book_side::ASK)
		{
			_asks.erase(cacheReplacement.oldPrice);
			_asks.emplace(std::move(cacheReplacement.newPrice), std::move(cacheReplacement.newEntry));
		}
		else
		{
			_bids.erase(cacheReplacement.oldPrice);
			_bids.emplace(std::move(cacheReplacement.newPrice), std::move(cacheReplacement.newEntry));
		}
	}

	order_book_state order_book_cache::snapshot() const
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		return order_book_state
		{
			to_vector<order_book_entry>(_asks, [](const std::pair<std::string, order_book_entry>& pair) { return pair.second; }),
			to_vector<order_book_entry>(_bids, [](const std::pair<std::string, order_book_entry>& pair) { return pair.second; })
		};
	}
}