#include "mathutils.h"

namespace mb
{
	bool approximately_equal(double a, double b)
	{
		// If close to zero: check for exactly zero then use absolute
		// Else use ULPs
		// https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/

		return a == b;
	}

	bool less_than(double a, double b)
	{
		return !approximately_equal(a, b) && a < b;
	}

	bool greater_than(double a, double b)
	{
		return !approximately_equal(a, b) && a > b;
	}
}