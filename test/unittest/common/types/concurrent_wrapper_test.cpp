#include <gtest/gtest.h>
#include <absl/synchronization/notification.h>

#include "common/types/concurrent_wrapper.h"

namespace mb::test
{
	using namespace std::chrono_literals;

	TEST(ConcurrentWrapper, CanCreateMultipleSharedLocks)
	{
		int readerCount = 0;
		constexpr int value = 5;

		concurrent_wrapper<int> wrapper{ value };

		std::vector<concurrent_wrapper<int>::shared_locked_object<int>> lockedObjects;

		auto read_task = [&readerCount, &wrapper, &lockedObjects]() {
			auto lockedValue = wrapper.shared_lock();
			readerCount++;
			EXPECT_EQ(value, *lockedValue);

			lockedObjects.push_back(std::move(lockedValue));
		};

		std::vector<std::thread> threads;

		threads.emplace_back(read_task);
		threads.emplace_back(read_task);
		threads.emplace_back(read_task);

		for (auto& thread : threads)
		{
			thread.join();
		}

		EXPECT_EQ(3, readerCount);
	}

	TEST(ConcurrentWrapper, SharedLockHasReadAccessOnly)
	{
		concurrent_wrapper<int> wrapper{ 5 };
		auto sharedLockedObject{ wrapper.shared_lock() };

		constexpr bool isConst = std::is_same<const int&, decltype(*sharedLockedObject)>::value;

		EXPECT_TRUE(isConst);
	}

	TEST(ConcurrentWrapper, UniqueLockHasReadAndWriteAccess)
	{
		concurrent_wrapper<int> wrapper{ 5 };
		auto uniqueLockedObject{ wrapper.unique_lock() };

		constexpr bool isNonConst = std::is_same<int&, decltype(*uniqueLockedObject)>::value;

		EXPECT_TRUE(isNonConst);
	}

	TEST(ConcurrentWrapper, CannotCreateUniqueLockWhileSharedLockExists)
	{
		concurrent_wrapper<int> wrapper{ 5 };
		std::vector<concurrent_wrapper<int>::shared_locked_object<int>> sharedLockedObjects;

		auto read_task = [&wrapper, &sharedLockedObjects]() {
			sharedLockedObjects.emplace_back(wrapper.shared_lock());
		};

		std::thread readThread{ read_task };
		readThread.join();

		absl::Notification done;
		auto write_task = [&wrapper, &done]() {
			auto uniqueLock{ wrapper.unique_lock() };
			done.Notify();
		};

		std::thread writeThread{ write_task };

		bool uniqueLockCreated = done.WaitForNotificationWithTimeout(absl::FromChrono(1s));
		writeThread.detach();

		EXPECT_FALSE(uniqueLockCreated);
	}

	TEST(ConcurrentWrapper, CannotCreateUniqueLockWhileOtherUniqueLockExists)
	{
		concurrent_wrapper<int> wrapper{ 5 };
		std::vector<concurrent_wrapper<int>::unique_locked_object<int>> uniqueLockedObjects;

		auto first_task = [&wrapper, &uniqueLockedObjects]() {
			uniqueLockedObjects.emplace_back(wrapper.unique_lock());
		};

		std::thread firstThread{ first_task };
		firstThread.join();

		absl::Notification done;
		auto second_task = [&wrapper, &done]() {
			auto uniqueLock{ wrapper.unique_lock() };
			done.Notify();
		};

		std::thread secondThread{ second_task };

		bool uniqueLockCreated = done.WaitForNotificationWithTimeout(absl::FromChrono(1s));
		secondThread.detach();

		EXPECT_FALSE(uniqueLockCreated);
	}

	TEST(ConcurrentWrapper, CannotCreateSharedLockWhileUniqueLockExists)
	{
		concurrent_wrapper<int> wrapper{ 5 };

		std::vector<concurrent_wrapper<int>::unique_locked_object<int>> uniqueLockedObjects;

		auto write_task = [&wrapper, &uniqueLockedObjects]() {
			uniqueLockedObjects.emplace_back(wrapper.unique_lock());
		};

		std::thread writeThread{ write_task };
		writeThread.join();

		absl::Notification done;
		auto read_task = [&wrapper, &done]() {
			auto sharedLock{ wrapper.shared_lock() };
			done.Notify();
		};

		std::thread readThread{ read_task };

		bool sharedLockCreated = done.WaitForNotificationWithTimeout(absl::FromChrono(1s));
		readThread.detach();

		EXPECT_FALSE(sharedLockCreated);
	}
}