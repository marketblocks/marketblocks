#pragma once

#include <vector>

class OrderBookEntry
{
private:
	double _price;
	double _volume;

public:
	OrderBookEntry(double price, double volume);

	double price() const { return _price; }
	double volume() const { return _volume; }
};

class OrderBookLevel
{
private:
	OrderBookEntry _askEntry;
	OrderBookEntry _bidEntry;

public:
	OrderBookLevel(OrderBookEntry askEntry, OrderBookEntry bidEntry);

	const OrderBookEntry& ask() const { return _askEntry; }
	const OrderBookEntry& bid() const { return _bidEntry; }
};

class OrderBookState
{
private:
	std::vector<OrderBookLevel> _levels;

public:
	OrderBookState(std::vector<OrderBookLevel> levels);

	int depth() const;
	const OrderBookLevel& level(int i) const;
};