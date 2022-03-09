#pragma once

#include <vector>
#include <cassert>

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

	public:
		constexpr order_book_entry(order_book_side side, double price, double volume)
			: _side{ side }, _price{ price }, _volume{ volume }
		{}

		constexpr order_book_side side() const noexcept { return _side; }
		constexpr double price() const noexcept { return _price; }
		constexpr double volume() const noexcept { return _volume; }
	};

	class order_book_level
	{
	private:
		order_book_entry _askEntry;
		order_book_entry _bidEntry;

	public:
		constexpr order_book_level(order_book_entry askEntry, order_book_entry bidEntry)
			: _askEntry{ std::move(askEntry) }, _bidEntry{ std::move(bidEntry) }
		{}

		constexpr const order_book_entry& ask() const noexcept { return _askEntry; }
		constexpr const order_book_entry& bid() const noexcept { return _bidEntry; }
	};

	class order_book_state
	{
	private:
		std::vector<order_book_level> _levels;

		constexpr std::vector<cb::order_book_level> convert_to_levels(const std::vector<cb::order_book_entry>& askEntries, const std::vector<cb::order_book_entry>& bidEntries)
		{
			std::vector<cb::order_book_level> levels;
			levels.reserve(askEntries.size());

			for (int i = 0; i < askEntries.size(); ++i)
			{
				levels.emplace_back(askEntries[i], bidEntries[i]);
			}

			return levels;
		}

	public:
		constexpr order_book_state(std::vector<order_book_level> levels)
			: _levels{ std::move(levels) }
		{}

		constexpr order_book_state(std::vector<order_book_entry> askEntries, std::vector<order_book_entry> bidEntries)
			: _levels{ convert_to_levels(askEntries, bidEntries) }
		{
		}

		constexpr size_t depth() const noexcept { return _levels.size(); }

		constexpr const order_book_level& level(int i) const
		{
			assert(i < depth());
			return _levels.at(i);
		}
	};
}