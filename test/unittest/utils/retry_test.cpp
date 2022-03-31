#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "common/utils/retry.h"

using testing::_;
using testing::MockFunction;
using testing::Return;

namespace mb::test
{
	TEST(Functional, RetryOnFailOnlyExecutesOnceOnSuccess)
	{
		constexpr int returnValue = 1;

		MockFunction<int()> mockAction;
		EXPECT_CALL(mockAction, Call()).WillOnce(Return(returnValue));

		auto toResult = [](int value) { return result<int>::success(std::move(value)); };

		EXPECT_EQ(retry_on_fail<int>(mockAction.AsStdFunction(), toResult, 3), returnValue);
	}

	TEST(Functional, RetryOnFailExecutesMaxRetriesThenThrowsOnFailure)
	{
		constexpr int maxRetries = 3;

		MockFunction<int()> mockAction;
		EXPECT_CALL(mockAction, Call()).Times(maxRetries + 1).WillRepeatedly(Return(0));

		auto toResult = [](int value) { return result<int>::fail(""); };

		EXPECT_THROW(retry_on_fail<int>(mockAction.AsStdFunction(), toResult, maxRetries), cb_exception);
	}
}