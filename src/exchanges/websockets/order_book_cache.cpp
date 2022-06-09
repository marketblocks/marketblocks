#include "order_book_cache.h"
#include "common/utils/containerutils.h"
#include "common/utils/mathutils.h"

namespace
{
	using namespace mb;

	template<typename Cache>
	void update_cache(Cache& cache, order_book_entry entry)
	{
		auto it = cache.find(entry);

		if (it != cache.end())
		{
			cache.erase(it);

			if (entry.volume() != 0.0)
			{
				cache.insert(std::move(entry));
			}
		}
		else
		{
			cache.emplace(std::move(entry));
		}
	}
}

namespace mb
{
	namespace internal
	{
		bool entry_less_than::operator()(const order_book_entry& l, const order_book_entry& r) const
		{
			return less_than(l.price(), r.price());
		}

		bool entry_greater_than::operator()(const order_book_entry& l, const order_book_entry& r) const
		{
			return greater_than(l.price(), r.price());
		}
	}

	order_book_cache::order_book_cache(ask_cache asks, bid_cache bids)
		: _asks{ std::move(asks) }, _bids{ std::move(bids) }, _mutex{}
	{}

	order_book_cache::order_book_cache(const order_book_cache& other)
		: _asks{ other._asks }, _bids{ other._bids }, _mutex{}
	{}

	order_book_cache::order_book_cache(order_book_cache&& other) noexcept
		: _asks{ std::move(other._asks) }, _bids{ std::move(other._bids) }, _mutex{}
	{}

	order_book_cache& order_book_cache::operator=(const order_book_cache& other)
	{
		_asks = other._asks;
		_bids = other._bids;

		return *this;
	}

	order_book_cache& order_book_cache::operator=(order_book_cache&& other) noexcept
	{
		_asks = std::move(other._asks);
		_bids = std::move(other._bids);

		return *this;
	}

	void order_book_cache::update_cache(order_book_entry entry)
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		entry.side() == order_book_side::ASK
			? ::update_cache(_asks, std::move(entry))
			: ::update_cache(_bids, std::move(entry));
	}

	order_book_state order_book_cache::snapshot(int depth) const
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		if (depth == 0)
		{
			depth = std::max(_asks.size(), _bids.size());
		}

		int askDepth = std::min(static_cast<int>(_asks.size()), depth);
		std::vector<order_book_entry> asks{ _asks.begin(), std::next(_asks.begin(), askDepth) };

		int bidDepth = std::min(static_cast<int>(_bids.size()), depth);
		std::vector<order_book_entry> bids{ _bids.begin(), std::next(_bids.begin(), bidDepth) };

		return order_book_state
		{
			std::move(asks),
			std::move(bids)
		};
	}
}