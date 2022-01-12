#pragma once

#include <string>
#include <map>
#include <mutex>

#include "common/types/containers/size_limited_map.h"
#include "trading/order_book.h"

class OrderBookCache
{
private:
	int count;
	SizeLimitedMap<std::string, OrderBookEntry> _asks;
	SizeLimitedMap<std::string, OrderBookEntry, std::greater<std::string>> _bids;
	mutable std::mutex _mutex;


public:
	OrderBookCache(int depth);

	OrderBookCache(const OrderBookCache&);
	OrderBookCache(OrderBookCache&&);

	OrderBookCache& operator=(const OrderBookCache&);
	OrderBookCache& operator=(OrderBookCache&&);

	void cache(std::string price, OrderBookEntry entry);

	OrderBookState snapshot() const;
};