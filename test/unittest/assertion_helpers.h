#pragma once

#include <gtest/gtest.h>

#include "common/types/result.h"

namespace cb::test
{
	template<typename T>
	void no_assert(const T&, const T&)
	{
	}

	template<typename T>
	void default_expect_eq(const T& lhs, const T& rhs)
	{
		EXPECT_EQ(lhs, rhs);
	}

	template<typename T, typename ValueAsserter>
	void assert_result_equal(const result<T>& lhs, const result<T>& rhs, ValueAsserter valueAsserter)
	{
		ASSERT_EQ(lhs.is_success(), rhs.is_success());

		if (lhs.is_success())
		{
			valueAsserter(lhs.value(), rhs.value());
		}
		else
		{
			ASSERT_EQ(lhs.error(), rhs.error());
		}
	}
}