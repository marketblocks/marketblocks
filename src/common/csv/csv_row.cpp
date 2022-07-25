#include "csv_row.h"

namespace mb
{
	csv_row::csv_row(std::vector<std::string> cells)
		: _cells{ std::move(cells) }
	{}

	const std::string& csv_row::get_cell(int index) const
	{
		assert(index < _cells.size());
		return _cells.at(index);
	}

	size_t csv_row::size() const noexcept
	{
		return _cells.size();
	}

	csv_row_iterator csv_row::begin() const
	{
		return csv_row_iterator{ _cells.begin() };
	}

	csv_row_iterator csv_row::end() const
	{
		return csv_row_iterator{ _cells.end() };
	}

	std::string csv_row::to_string() const
	{
		if (_cells.size() == 0)
		{
			return "";
		}

		std::string result{ _cells.front() };

		for (int i = 1; i < _cells.size(); ++i)
		{
			result.append(",");
			result.append(_cells[i]);
		}

		return result;
	}

	csv_row parse_row(std::string_view line)
	{
		std::vector<std::string> cells;
		std::string cell;
		std::stringstream lineStream{ line.data() };

		while (std::getline(lineStream, cell, ','))
		{
			cells.push_back(cell);
		}

		if (!lineStream && cell.empty())
		{
			cells.push_back("");
		}

		return csv_row{ std::move(cells) };
	}
}