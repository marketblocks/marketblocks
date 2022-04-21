#include <gtest/gtest.h>

#include "common/csv/csv_row.h"

namespace mb::test
{
	TEST(CsvRow, ToString)
	{
		csv_row row{ {"This", "is", "a", "", "test", "12345"} };
		ASSERT_EQ(row.to_string(), "This,is,a,,test,12345");
	}
}