#include "csv.h"

#include <sstream>

namespace mb
{
	csv_document::csv_document(std::vector<csv_row> rows)
		: _rows{ std::move(rows) }
	{}

	const csv_row& csv_document::get_row(int index) const
	{
		assert(index < _rows.size());
		return _rows.at(index);
	}

	size_t csv_document::row_count() const noexcept
	{
		return _rows.size();
	}

	csv_iterator csv_document::begin() const
	{
		return csv_iterator{ _rows.begin() };
	}

	csv_iterator csv_document::end() const
	{
		return csv_iterator{ _rows.end() };
	}

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