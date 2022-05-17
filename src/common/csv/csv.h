#pragma once

#include <string>
#include <vector>
#include <cassert>

#include "csv_row.h"
#include "common/types/basic_iterator.h"
#include "common/file/file.h"

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

	template<typename T>
	T from_csv_row(const csv_row& row)
	{
		static_assert(sizeof(T) == 0, "No specialization of from_csv_row found");
	}

	template<typename T>
	csv_row to_csv_row(const T& data)
	{
		static_assert(sizeof(T) == 0, "No specialization of from_csv_row found");
	}

	template<typename T>
	std::vector<T> read_csv_file(const std::filesystem::path& path)
	{
		std::vector<T> result;
		stream_file(path, [&result](std::string_view line)
			{
				csv_row row{ parse_row(line) };
				result.emplace_back(from_csv_row<T>(row));
			});

		return result;
	}

	template<typename T, typename CsvData>
	void write_to_csv_file(const std::filesystem::path& path, const CsvData& data, const std::vector<std::string>& headers = {})
	{
		file_handler fileHandler{ file_handler::write(path) };

		if (!headers.empty())
		{
			fileHandler.stream() << csv_row{ headers }.to_string() << std::endl;
		}

		for (const T& item : data)
		{
			fileHandler.stream() << to_csv_row(item).to_string() << std::endl;
		}
	}
}
