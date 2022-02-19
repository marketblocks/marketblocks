#pragma once

#include <gtest/gtest.h>

#include "common/types/result.h"

namespace
{
	template<typename T>
	void default_assert_eq(const T& lhs, const T& rhs)
	{
		ASSERT_EQ(lhs, rhs);
	}
}

namespace cb::test
{
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

	template<typename T>
	void assert_result_equal(const result<T>& lhs, const result<T>& rhs)
	{
		assert_result_equal(lhs, rhs, default_assert_eq<T>);
	}
}