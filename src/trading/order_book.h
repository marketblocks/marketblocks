#pragma once

#include <vector>

enum class OrderBookSide
{
	ASK,
	BID
};

class OrderBookEntry
{
private:
	OrderBookSide _side;
	double _price;
	double _volume;
	double _timeStamp;

public:
	OrderBookEntry(OrderBookSide side, double price, double volume, double timeStamp);

	OrderBookSide side() const { return _side; }
	double price() const { return _price; }
	double volume() const { return _volume; }
	double time_stamp() const { return _timeStamp; }
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
	OrderBookState(std::vector<OrderBookEntry> askEntries, std::vector<OrderBookEntry> bidEntries);

	int depth() const;
	const OrderBookLevel& level(int i) const;
};