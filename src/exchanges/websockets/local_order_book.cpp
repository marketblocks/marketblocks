#include "local_order_book.h"
#include "common/exceptions/cb_exception.h"

namespace cb
{
	order_book_state local_order_book::get_order_book(const tradable_pair& pair) const
	{
		auto it = _orderBookCaches.find(pair.to_standard_string());

		if (it == _orderBookCaches.end())
		{
			throw cb_exception{ std::format("Order book not subscribed for pair {}", pair.to_standard_string()) };
		}

		return it->second.snapshot();
	}

	bool local_order_book::is_subscribed(const tradable_pair& pair) const
	{
		return _orderBookCaches.contains(pair.to_standard_string());
	}

	void local_order_book::initialise_book(tradable_pair pair, std::vector<cache_entry> asks, std::vector<cache_entry> bids)
	{
		_orderBookCaches.emplace(pair.to_standard_string(), order_book_cache{std::move(asks), std::move(bids)});
		_onMessage(pair);
	}

	void local_order_book::update_book(const tradable_pair& pair, cache_entry cacheEntry)
	{
		auto cacheIterator = _orderBookCaches.find(pair.to_standard_string());
		if (cacheIterator != _orderBookCaches.end())
		{
			order_book_cache& cache{ cacheIterator->second };
			cache.cache(std::move(cacheEntry));
			_onMessage(pair);
		}
	}

	void local_order_book::replace_in_book(const tradable_pair& pair, cache_replacement cacheReplacement)
	{
		auto cacheIterator = _orderBookCaches.find(pair.to_standard_string());
		if (cacheIterator != _orderBookCaches.end())
		{
			order_book_cache& cache{ cacheIterator->second };
			cacheIterator->second.replace(std::move(cacheReplacement));
			_onMessage(pair);
		}
	}
}