#pragma once

#include <string>
#include <map>
#include <mutex>

#include "trading/order_book.h"
#include "common/utils/stringutils.h"

class OrderBookCache
{
public:
	int _depth;
	std::map<std::string, OrderBookEntry, numeric_string_less> _asks;
	std::map<std::string, OrderBookEntry, numeric_string_greater> _bids;
	mutable std::mutex _mutex;

public:
	OrderBookCache(int depth);

	OrderBookCache(const OrderBookCache&);
	OrderBookCache(OrderBookCache&&);

	OrderBookCache& operator=(const OrderBookCache&);
	OrderBookCache& operator=(OrderBookCache&&);

	void cache(std::string price, OrderBookEntry entry);
	void replace(const std::string& oldPrice, std::string newPrice, OrderBookEntry newEntry);

	OrderBookState snapshot() const;
};