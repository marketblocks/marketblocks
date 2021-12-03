#include <gtest/gtest.h>

#include "utils/mathutils.h"

TEST(MathUtils, CalculatePercentageDiffValidArgs)
{
	constexpr double tolerance = 1e-3;

	EXPECT_NEAR(calculate_percentage_diff(4, 5), 25, tolerance);
	EXPECT_NEAR(calculate_percentage_diff(7, 9), 28.571, tolerance);
	EXPECT_NEAR(calculate_percentage_diff(7.5, 10), 33.333, tolerance);
	EXPECT_NEAR(calculate_percentage_diff(7, 10.0), 42.857, tolerance);

	EXPECT_NEAR(calculate_percentage_diff(5, 4), -20, tolerance);
	EXPECT_NEAR(calculate_percentage_diff(9, 7), -22.222, tolerance);
	EXPECT_NEAR(calculate_percentage_diff(10, 7.5), -25.0, tolerance);
	EXPECT_NEAR(calculate_percentage_diff(10.0, 7), -30.0, tolerance);
}