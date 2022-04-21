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

		std::string csvString{ read_file(csv_test_data_path("csv_test_file.csv")) };
		csv_document csv{ parse_csv(csvString) };

		ASSERT_EQ(expectedRows.size(), csv.size());

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
}