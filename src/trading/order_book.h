#pragma once

#include <vector>
#include <cassert>

#include "trading/trading_constants.h"
#include "common/exceptions/mb_exception.h"

namespace mb
{
	enum class order_book_side
	{
		ASK,
		BID
	};

	class order_book_entry
	{
	private:
		double _price;
		double _volume;
		order_book_side _side;

	public:
		constexpr order_book_entry(double price, double volume, order_book_side side)
			: _price{ price }, _volume{ volume }, _side{ side }
		{}

		constexpr double price() const noexcept { return _price; }
		constexpr double volume() const noexcept { return _volume; }
		constexpr order_book_side side() const noexcept { return _side; }
	};

	class order_book_state
	{
	private:
		std::vector<order_book_entry> _asks;
		std::vector<order_book_entry> _bids;

	public:
		order_book_state(std::vector<order_book_entry> asks, std::vector<order_book_entry> bids);

		const std::vector<order_book_entry>& asks() const noexcept { return _asks; }
		const std::vector<order_book_entry>& bids() const noexcept { return _bids; }

		constexpr int depth() const { return std::max(_asks.size(), _bids.size()); }
	};
	
	const order_book_entry& get_best_entry(const std::vector<order_book_entry>& entries);
	const order_book_entry& select_best_entry(const order_book_state& orderBook, trade_action action);
}