#include "order_book_cache.h"
#include "common/utils/containerutils.h"

namespace
{
	template<typename OrderBookMap>
	void ensure_depth(OrderBookMap& map, int depth)
	{
		if (depth == 0)
		{
			return;
		}

		while (map.size() > depth)
		{
			map.erase(std::prev(map.end()));
		}
	}
}

namespace cb
{
	order_book_cache::order_book_cache(ask_map asks, bid_map bids, int depth)
		: _asks{ std::move(asks) }, _bids{ std::move(bids) }, _depth{ depth }, _mutex{}
	{
		ensure_depth(_asks, depth);
		ensure_depth(_bids, depth);
	}

	order_book_cache::order_book_cache(const order_book_cache& other)
		: _asks{ other._asks }, _bids{ other._bids }, _depth{ other._depth }, _mutex{}
	{}

	order_book_cache::order_book_cache(order_book_cache&& other) noexcept
		: _asks{ std::move(other._asks) }, _bids{ std::move(other._bids) }, _depth{ std::move(other._depth) }, _mutex{}
	{
	}

	order_book_cache& order_book_cache::operator=(const order_book_cache& other)
	{
		_asks = other._asks;
		_bids = other._bids;
		_depth = other._depth;

		return *this;
	}

	order_book_cache& order_book_cache::operator=(order_book_cache&& other) noexcept
	{
		_asks = std::move(other._asks);
		_bids = std::move(other._bids);
		_depth = std::move(other._depth);

		return *this;
	}

	void order_book_cache::update_cache(order_book_cache_entry cacheEntry)
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		if (cacheEntry.side == order_book_side::ASK)
		{
			_asks[std::move(cacheEntry.price)] = std::move(cacheEntry.entry);
			ensure_depth(_asks, _depth);
		}
		else
		{
			_bids[std::move(cacheEntry.price)] = std::move(cacheEntry.entry);
			ensure_depth(_bids, _depth);
		}
	}

	void order_book_cache::remove(std::string_view oldPrice, order_book_side side)
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		if (side == order_book_side::ASK)
		{
			_asks.erase(std::string{ oldPrice });
		}
		else
		{
			_bids.erase(std::string{ oldPrice });
		}
	}

	order_book_state order_book_cache::snapshot(int depth) const
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		std::vector<order_book_entry> asks;
		asks.reserve(_asks.size());

		std::vector<order_book_entry> bids;
		bids.reserve(_bids.size());

		int snapshotDepth = depth == 0 ? _depth : depth;
		auto askIt = _asks.begin();
		auto bidIt = _bids.begin();

		for (int i = 0; i < snapshotDepth; ++i)
		{
			if (askIt != _asks.end())
			{
				asks.emplace_back(askIt->second);
				++askIt;
			}

			if (bidIt != _bids.end())
			{
				bids.emplace_back(bidIt->second);
				++bidIt;
			}
		}

		return order_book_state
		{
			std::move(asks),
			std::move(bids)
		};
	}
}