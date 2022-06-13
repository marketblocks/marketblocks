#include <gtest/gtest.h>

#include "common/utils/mathutils.h"

namespace mb
{
	TEST(MathUtils, CalculatePercentageDiffValidArgs)
	{
		EXPECT_DOUBLE_EQ(calculate_percentage_diff(4, 5), 25);
		EXPECT_DOUBLE_EQ(calculate_percentage_diff(7, 9), 28.571);
		EXPECT_DOUBLE_EQ(calculate_percentage_diff(7.5, 10), 33.333);
		EXPECT_DOUBLE_EQ(calculate_percentage_diff(7, 10.0), 42.857);

		EXPECT_DOUBLE_EQ(calculate_percentage_diff(5, 4), -20);
		EXPECT_DOUBLE_EQ(calculate_percentage_diff(9, 7), -22.222);
		EXPECT_DOUBLE_EQ(calculate_percentage_diff(10, 7.5), -25.0);
		EXPECT_DOUBLE_EQ(calculate_percentage_diff(10.0, 7), -30.0);
	}

	TEST(MathUtils, DoubleEqualTrue)
	{
		EXPECT_TRUE(double_equal(0.0, 0.0));
		EXPECT_TRUE(double_equal(1.2345678e30, 1.2345678e30 + 2*DBL_EPSILON));
		EXPECT_TRUE(double_equal(1.0, 0.1*10));
		EXPECT_TRUE(double_equal(1.0e-30, 1.1e-30));
	}

	TEST(MathUtils, DoubleEqualFalse)
	{
		EXPECT_FALSE(double_equal(0.0, 1.0));
		EXPECT_FALSE(double_equal(1.0e-15, 2.0e-15));
		EXPECT_FALSE(double_equal(1e30, 1e29));
	}
}