#include "local_order_book.h"
#include "common/exceptions/mb_exception.h"

namespace mb
{
	local_order_book::local_order_book()
		: _messageQueue{}, _orderBookCaches{}, _mutex{}
	{}

	local_order_book::local_order_book(const local_order_book& other)
		: _messageQueue{}, _orderBookCaches{}, _mutex{}
	{
		std::lock_guard<std::mutex> lock{ other._mutex };

		_messageQueue = other._messageQueue;
		_orderBookCaches = other._orderBookCaches;
	}

	local_order_book::local_order_book(local_order_book&& other) noexcept
		: _messageQueue{}, _orderBookCaches{}, _mutex{}
	{
		std::lock_guard<std::mutex> lock{ other._mutex };

		_messageQueue = std::move(other._messageQueue);
		_orderBookCaches = std::move(other._orderBookCaches);
	}

	local_order_book& local_order_book::operator=(const local_order_book& other)
	{
		std::lock_guard<std::mutex> lock{ _mutex };
		std::lock_guard<std::mutex> otherLock{ other._mutex };

		_messageQueue = other._messageQueue;
		_orderBookCaches = other._orderBookCaches;

		return *this;
	}

	local_order_book& local_order_book::operator=(local_order_book&& other) noexcept
	{
		std::lock_guard<std::mutex> lock{ _mutex };
		std::lock_guard<std::mutex> otherLock{ other._mutex };

		_messageQueue = std::move(other._messageQueue);
		_orderBookCaches = std::move(other._orderBookCaches);

		return *this;
	}

	order_book_state local_order_book::get_order_book(const tradable_pair& pair, int depth) const
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		auto it = _orderBookCaches.find(pair);

		if (it == _orderBookCaches.end())
		{
			throw mb_exception{ std::format("Order book not subscribed for pair {}", pair.to_string('/'))};
		}

		return it->second.snapshot(depth);
	}

	bool local_order_book::is_subscribed(const tradable_pair& pair) const
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		return _orderBookCaches.contains(pair);
	}

	void local_order_book::initialise_book(const tradable_pair& pair, ask_map asks, bid_map bids, int depth)
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		_orderBookCaches.emplace(pair, order_book_cache{std::move(asks), std::move(bids), depth});
		_messageQueue.push(tradable_pair{ pair });
	}

	void local_order_book::update_book(const tradable_pair& pair, order_book_cache_entry cacheEntry)
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		auto cacheIterator = _orderBookCaches.find(pair);
		if (cacheIterator != _orderBookCaches.end())
		{
			cacheIterator->second.update_cache(std::move(cacheEntry));
			_messageQueue.push(tradable_pair{ pair });
		}
	}

	void local_order_book::remove_from_book(const tradable_pair& pair, std::string_view price, order_book_side side)
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		auto cacheIterator = _orderBookCaches.find(pair);
		if (cacheIterator != _orderBookCaches.end())
		{
			cacheIterator->second.remove(price, side);
			_messageQueue.push(tradable_pair{ pair });
		}
	}

	void local_order_book::replace_in_book(const tradable_pair& pair, std::string_view oldPrice, order_book_cache_entry cacheEntry)
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		auto cacheIterator = _orderBookCaches.find(pair);
		if (cacheIterator != _orderBookCaches.end())
		{
			cacheIterator->second.remove(oldPrice, cacheEntry.side);
			cacheIterator->second.update_cache(std::move(cacheEntry));
			_messageQueue.push(tradable_pair{ pair });
		}
	}
}