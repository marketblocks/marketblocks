#include <cmath>
#include <limits>

#include "mathutils.h"

namespace mb
{
	bool double_equal(double a, double b)
	{
		// If close to zero: check for exactly zero then use absolute
		// Else use ULPs
		// https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/

		if (std::isnan(a) || std::isnan(b))
		{
			return false;
		}

		double diff = std::abs(a - b);
		if (diff <= 4 * std::numeric_limits<double>::epsilon())
		{
			return true;
		}

		a = std::abs(a);
		b = std::abs(b);

		double largest = (b > a) ? b : a;

		return diff <= largest * std::numeric_limits<double>::epsilon();
	}

	bool double_less_than(double a, double b)
	{
		return !double_equal(a, b) && a < b;
	}

	bool double_greater_than(double a, double b)
	{
		return !double_equal(a, b) && a > b;
	}
}