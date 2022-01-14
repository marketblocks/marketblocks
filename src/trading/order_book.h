#pragma once

#include <vector>

namespace cb
{
	enum class order_book_side
	{
		ASK,
		BID
	};

	class order_book_entry
	{
	private:
		order_book_side _side;
		double _price;
		double _volume;
		double _timeStamp;

	public:
		order_book_entry(order_book_side side, double price, double volume, double timeStamp);

		order_book_side side() const { return _side; }
		double price() const { return _price; }
		double volume() const { return _volume; }
		double time_stamp() const { return _timeStamp; }
	};

	class order_book_level
	{
	private:
		order_book_entry _askEntry;
		order_book_entry _bidEntry;

	public:
		order_book_level(order_book_entry askEntry, order_book_entry bidEntry);

		const order_book_entry& ask() const { return _askEntry; }
		const order_book_entry& bid() const { return _bidEntry; }
	};

	class order_book_state
	{
	private:
		std::vector<order_book_level> _levels;

	public:
		order_book_state(std::vector<order_book_level> levels);
		order_book_state(std::vector<order_book_entry> askEntries, std::vector<order_book_entry> bidEntries);

		int depth() const;
		const order_book_level& level(int i) const;
	};
}