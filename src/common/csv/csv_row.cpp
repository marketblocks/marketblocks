#include "csv_row.h"

namespace mb
{
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