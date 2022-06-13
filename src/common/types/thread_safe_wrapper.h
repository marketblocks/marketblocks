#pragma once

#include <shared_mutex>

namespace mb
{
	template<typename T, typename Lock>
	class locked_object
	{
	private:
		T* _item;
		Lock _lock;

	public:
		locked_object(T* item, Lock lock)
			: _item{ item }, _lock{ std::move(lock) }
		{}

		const T* operator->() const { return _item; }
		T* operator->() { return _item; }

		const T& operator*() const { return *_item; }
		T& operator*() { return *_item; }
	};

	template<typename T>
	using exclusive_locked_object = locked_object<T, std::unique_lock<std::shared_mutex>>;

	template<typename T>
	using shared_locked_object = locked_object<const T, std::shared_lock<std::shared_mutex>>;

	template<typename T>
	class thread_safe_wrapper
	{
	private:
		T _item;
		mutable std::shared_mutex _mutex;

	public:
		thread_safe_wrapper()
			: _item{}, _mutex{}
		{}

		thread_safe_wrapper(const T&& item)
			: _item{ std::forward<T>(item) }, _mutex{}
		{}

		exclusive_locked_object<T> unique_lock()
		{
			return exclusive_locked_object<T>{ &_item, std::unique_lock<std::shared_mutex>{ _mutex } };
		}

		shared_locked_object<T> shared_lock() const
		{
			return shared_locked_object<T>{ &_item, std::shared_lock<std::shared_mutex>{ _mutex } };
		}
	};
}