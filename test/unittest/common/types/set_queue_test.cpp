#include <gtest/gtest.h>

#include "common/types/set_queue.h"

namespace mb::test
{
	TEST(SetQueue, PushAddsItemsToQueue)
	{
		set_queue<int> queue;

		queue.push(1);
		queue.push(2);
		queue.push(3);

		ASSERT_EQ(queue.size(), 3);
	}

	TEST(SetQueue, PopReturnsItemAtFrontOfQueue)
	{
		set_queue<int> queue;

		queue.push(1);
		queue.push(2);
		queue.push(3);

		ASSERT_EQ(queue.pop(), 1);
	}

	TEST(SetQueue, PopRemovesItemAtFrontOfQueue)
	{
		set_queue<int> queue;

		queue.push(1);
		queue.push(2);
		queue.push(3);

		queue.pop();

		ASSERT_EQ(queue.size(), 2);
		ASSERT_EQ(queue.pop(), 2);
	}

	TEST(SetQueue, PushDoesNotAddIfItemAlreadyExists)
	{
		set_queue<int> queue;

		queue.push(1);
		queue.push(1);
		queue.push(1);

		ASSERT_EQ(queue.size(), 1);
	}

	TEST(SetQueue, EmptyReturnsTrueIfQueueIsEmpty)
	{
		set_queue<int> queue;

		ASSERT_TRUE(queue.empty());
	}

	TEST(SetQueue, EmptyReturnsFalseIfQueueIsNotEmpty)
	{
		set_queue<int> queue;

		queue.push(1);

		ASSERT_FALSE(queue.empty());
	}
}