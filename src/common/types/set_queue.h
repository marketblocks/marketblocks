#pragma once

#include <queue>
#include <unordered_set>
#include <mutex>

namespace cb
{
	template<typename T>
	class set_queue
	{
	private:
		std::queue<T> _queue;
		std::unordered_set<std::reference_wrapper<T>, std::hash<T>, std::equal_to<T>> _set;
		mutable std::mutex _mutex;

	public:
		set_queue()
			: _queue{}, _set{}, _mutex{}
		{}

		set_queue(const set_queue<T>& other)
			: _queue{}, _set{}, _mutex{}
		{
			std::lock_guard<std::mutex> lock{ other._mutex };
			_queue = other._queue;
			_set = other._set;
		}

		set_queue(set_queue<T>&& other) noexcept
			: _queue{}, _mutex{}
		{
			std::lock_guard<std::mutex> lock{ other._mutex };
			_queue = std::move(other._queue);
			_set = std::move(other._set);
		}

		set_queue<T>& operator=(const set_queue<T>& other)
		{
			std::lock_guard<std::mutex> lock{ _mutex };
			std::lock_guard<std::mutex> otherLock{ other._mutex };

			_queue = other._queue;
			_set = other._set;

			return *this;
		}

		set_queue<T>& operator=(set_queue<T>&& other) noexcept
		{
			std::lock_guard<std::mutex> lock{ _mutex };
			std::lock_guard<std::mutex> otherLock{ other._mutex };

			_queue = std::move(other._queue);
			_set = std::move(other._set);

			return *this;
		}

		void push(T&& item)
		{
			std::lock_guard<std::mutex> lock{ _mutex };

			if (_set.find(item) != _set.end())
			{
				return;
			}

			_queue.push(std::forward<T>(item));
			_set.emplace(std::ref(_queue.back()));
		}

		bool empty() const noexcept
		{
			std::lock_guard<std::mutex> lock{ _mutex };

			return _queue.empty();
		}

		size_t size() const
		{
			std::lock_guard<std::mutex> lock{ _mutex };

			return _queue.size();
		}

		T pop()
		{
			std::lock_guard<std::mutex> lock{ _mutex };

			_set.erase(_queue.front());

			T item{ std::move(_queue.front()) };
			_queue.pop();

			return item;
		}
	};
};