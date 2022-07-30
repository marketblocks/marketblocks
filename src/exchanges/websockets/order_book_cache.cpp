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
		}

		if (entry.volume() > 0.0)
		{
			cache.insert(std::move(entry));
		}
	}
}

namespace mb
{
	namespace internal
	{
		bool entry_less_than::operator()(const order_book_entry& l, const order_book_entry& r) const
		{
			return double_less_than(l.price(), r.price());
		}

		bool entry_greater_than::operator()(const order_book_entry& l, const order_book_entry& r) const
		{
			return double_greater_than(l.price(), r.price());
		}
	}

	order_book_cache::order_book_cache(std::time_t timeStamp, ask_cache asks, bid_cache bids)
		: _lastUpdate{ timeStamp }, _asks{ std::move(asks) }, _bids{std::move(bids)}
	{}

	void order_book_cache::update_cache(std::time_t timeStamp, order_book_entry entry)
	{
		_lastUpdate = timeStamp;

		entry.side() == order_book_side::ASK
			? ::update_cache(_asks, std::move(entry))
			: ::update_cache(_bids, std::move(entry));
	}

	order_book_state order_book_cache::snapshot(int depth) const
	{
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
			_lastUpdate,
			std::move(asks),
			std::move(bids)
		};
	}

	order_book_cache from_snapshot(const order_book_state& snapshot)
	{
		return order_book_cache
		{
			snapshot.time_stamp(),
			ask_cache{ snapshot.asks().begin(), snapshot.asks().end() },
			bid_cache{ snapshot.bids().begin(), snapshot.bids().end() }
		};
	}
}