#pragma once

#include <vector>
#include <string>
#include <sstream>
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
		csv_row(std::vector<std::string> cells);

		const std::string& get_cell(int index) const;
		size_t size() const noexcept;
		csv_row_iterator begin() const;
		csv_row_iterator end() const;
		std::string to_string() const;
	};

	csv_row parse_row(std::string_view line);
}