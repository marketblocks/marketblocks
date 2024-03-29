#include <gtest/gtest.h>

#include "common/csv/csv.h"
#include "test_data/test_data_constants.h"
#include "common/file/file.h"

namespace mb::test
{
	TEST(CSV, CorrectlyParsesCsvString)
	{
		std::vector<csv_row> expectedRows
		{
			csv_row{ { "r1c1", "r1c2", "r1c3" } },
			csv_row{ { "r2c1", "1234", "" } },
			csv_row{ { "5678", "", "" } },
			csv_row{ { "", "", "" } },
			csv_row{ { "/\"r5c1\"\\", "r5c2", "1234.5678" } }
		};

		std::filesystem::path path{ TEST_DATA_FOLDER };
		path /= "csv_test/csv_test_file.csv";
		std::string csvString{ read_file(path) };
		csv_document csv{ parse_csv(csvString) };

		ASSERT_EQ(expectedRows.size(), csv.row_count());

		for (int i = 0; i < expectedRows.size(); ++i)
		{
			const csv_row& expectedRow = expectedRows[i];
			const csv_row& actualRow = csv.get_row(i);

			ASSERT_EQ(expectedRow.size(), actualRow.size());

			for (int j = 0; j < expectedRow.size(); j++)
			{
				EXPECT_EQ(expectedRow.get_cell(j), actualRow.get_cell(j));
			}
		}
	}

	TEST(CSV, CreatesOneRowOneColumnForStringWithNoSeparator)
	{
		std::string csvString{ "ThisIsAStringWithNoSeperator" };
		csv_document csv{ parse_csv(csvString) };

		EXPECT_EQ(1, csv.row_count());
		
		csv_row row{ csv.get_row(0) };

		EXPECT_EQ(1, row.size());
		EXPECT_EQ(csvString, row.get_cell(0));
	}

	TEST(CSV, CreatesEmptyDocumentForEmptyString)
	{
		std::string csvString{ "" };
		csv_document csv{ parse_csv(csvString) };

		EXPECT_EQ(0, csv.row_count());
	}
}