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
			rows.emplace_back(parse_row(line));
		}

		return csv_document{ std::move(rows) };
	}
}