#pragma once

#include <vector>
#include <string>
#include <cassert>

#include "common/types/basic_iterator.h"

namespace mb
{
	using csv_row_iterator = basic_iterator<std::string, std::vector<std::string>::const_iterator>;

	class csv_row
	{
	private:
		std::vector<std::string> _cells;

	public:
		constexpr explicit csv_row(std::vector<std::string> cells)
			: _cells{ std::move(cells) }
		{}

		constexpr const std::string& get_cell(int index) const
		{
			assert(index < _cells.size());
			return _cells.at(index);
		}

		constexpr size_t size() const noexcept
		{
			return _cells.size();
		}

		constexpr csv_row_iterator begin() const
		{
			return csv_row_iterator{ _cells.begin() };
		}

		constexpr csv_row_iterator end() const
		{
			return csv_row_iterator{ _cells.end() };
		}

		constexpr std::string to_string() const
		{
			std::string result{ _cells.front() };

			for (int i = 1; i < _cells.size(); ++i)
			{
				result.append(",");
				result.append(_cells[i]);
			}

			return result;
		}
	};
}