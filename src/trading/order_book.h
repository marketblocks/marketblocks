#pragma once

#include <vector>
#include <cassert>

#include "trading/trading_constants.h"
#include "common/exceptions/cb_exception.h"

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

	public:
		constexpr order_book_entry()
			: _price{}, _volume{}
		{}

		constexpr order_book_entry(double price, double volume)
			: _price{ price }, _volume{ volume }
		{}

		constexpr double price() const noexcept { return _price; }
		constexpr double volume() const noexcept { return _volume; }
	};

	class order_book_state
	{
	private:
		std::vector<order_book_entry> _asks;
		std::vector<order_book_entry> _bids;

	public:
		constexpr order_book_state(std::vector<order_book_entry> asks, std::vector<order_book_entry> bids)
			: _asks{ std::move(asks) }, _bids{ std::move(bids) }
		{
		}

		constexpr const std::vector<order_book_entry>& asks() const noexcept { return _asks; }
		constexpr const std::vector<order_book_entry>& bids() const noexcept { return _bids; }

		constexpr int depth() const { return std::max(_asks.size(), _bids.size()); }
	};
	
	constexpr const order_book_entry& get_best_entry(const std::vector<order_book_entry>& entries)
	{
		if (entries.size() == 0)
		{
			throw cb_exception{ "Order Book entries are empty" };
		}

		return entries[0];
	}

	constexpr const order_book_entry& select_best_entry(const order_book_state& orderBook, trade_action action)
	{
		return action == trade_action::BUY
			? get_best_entry(orderBook.asks())
			: get_best_entry(orderBook.bids());
	}
}