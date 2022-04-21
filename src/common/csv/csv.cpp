#include "csv.h"

#include <sstream>

namespace mb
{
	csv_document parse_csv(std::string_view csvString)
	{
		std::vector<csv_row> rows;
		std::stringstream inputStream{ csvString.data() };
		std::string line;

		while (std::getline(inputStream, line))
		{
			std::vector<std::string> cells;
			std::stringstream lineStream{ line };
			std::string cell;

			while (std::getline(lineStream, cell, ','))
			{
				cells.push_back(cell);
			}

			if (!lineStream && cell.empty())
			{
				cells.push_back("");
			}

			rows.emplace_back(std::move(cells));
		}

		return csv_document{ std::move(rows) };
	}
}