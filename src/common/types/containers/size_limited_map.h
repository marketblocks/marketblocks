#pragma once

#include <map>

template<typename Key, typename Value, typename Comparer = std::less<Key>>
class SizeLimitedMap
{
private:
	std::map<Key, Value, Comparer> _map;
	int _size;

	void ensure_size()
	{
		while (_map.size() > _size)
		{
			_map.erase(--_map.end());
		}
	}

public:
	SizeLimitedMap(int size)
		: _map{}, _size{ size }
	{}

	int size() const { return _size; }

	void insert(Key&& key, Value&& value)
	{
		_map.insert(std::forward<Key>(key), std::forward<Value>(value));
		ensure_size();
	}

	template<typename... Args>
	void emplace(Args&&... args)
	{
		_map.emplace(std::forward<Args>(args)...);
		ensure_size();
	}

	void erase(int i)
	{
		_map.erase(i);
	}

	Value& at(int i) const
	{
		return _map.at(i);
	}

	Value& operator[](int i) const
	{
		return _map[i];
	}

	std::map<Key, Value>::const_iterator begin() const
	{
		return _map.begin();
	}

	std::map<Key, Value>::const_iterator end() const
	{
		return _map.end();
	}
};