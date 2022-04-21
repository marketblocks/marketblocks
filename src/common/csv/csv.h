#pragma once

#include <string>
#include <vector>
#include <cassert>

#include "csv_row.h"
#include "common/types/basic_iterator.h"

namespace mb
{
	using csv_iterator = basic_iterator<csv_row, std::vector<csv_row>::const_iterator>;

	class csv_document
	{
	private:
		std::vector<csv_row> _rows;

	public:
		constexpr explicit csv_document(std::vector<csv_row> rows)
			: _rows{ std::move(rows) }
		{}

		constexpr const csv_row& get_row(int index) const
		{
			assert(index < _rows.size());
			return _rows.at(index);
		}

		constexpr size_t size() const noexcept
		{
			return _rows.size();
		}

		constexpr csv_iterator begin() const
		{
			return csv_iterator{ _rows.begin() };
		}

		constexpr csv_iterator end() const
		{
			return csv_iterator{ _rows.end() };
		}
	};

	csv_document parse_csv(std::string_view csvString);
}
