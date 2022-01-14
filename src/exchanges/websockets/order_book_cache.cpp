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

namespace cb
{
	order_book_cache::order_book_cache(int depth)
		: _depth{ depth }, _asks{}, _bids{}, _mutex{}
	{}

	order_book_cache::order_book_cache(const order_book_cache& other)
		: _depth{ other._depth }, _asks{ other._asks }, _bids{ other._bids }, _mutex{}
	{}

	order_book_cache::order_book_cache(order_book_cache&& other)
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

	order_book_cache& order_book_cache::operator=(order_book_cache&& other)
	{
		_depth = std::move(other._depth);
		_asks = std::move(other._asks);
		_bids = std::move(other._bids);

		return *this;
	}

	void order_book_cache::cache(std::string price, order_book_entry entry)
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		if (entry.side() == order_book_side::ASK)
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

	void order_book_cache::replace(const std::string& oldPrice, std::string newPrice, order_book_entry newEntry)
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		if (newEntry.side() == order_book_side::ASK)
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