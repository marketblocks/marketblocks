#include <gtest/gtest.h>

namespace mb::test
{
	MATCHER_P(IsOrderRequest, expected, "")
	{
		return
			expected.order_type() == arg.order_type() &&
			expected.pair() == arg.pair() &&
			expected.action() == arg.action();
	}
}