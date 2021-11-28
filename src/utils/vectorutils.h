#pragma once

#include <vector>
#include <functional>
#include <algorithm>

template<typename T, typename Predicate>
std::vector<T> copy_where(const std::vector<T>& source, Predicate p)
{
	std::vector<T> copied;
	std::copy_if(source.begin(), source.end(), std::back_inserter(copied), p);

	return copied;
}

template<typename Key, typename Value, typename Source, typename KeySelector, typename ValueSelector>
std::unordered_map<Key, Value> to_unordered_map(const std::vector<Source>& source, KeySelector keySelector, ValueSelector valueSelector)
{
	std::unordered_map<Key, Value> map;
	map.reserve(source.size());

	for (const Source& item : source)
	{
		map.emplace(keySelector(item), valueSelector(item));
	}
	return map;
}