#include <unordered_map>

template<typename Key, typename Value, typename Container, typename KeySelector, typename ValueSelector>
std::unordered_map<Key, Value> to_unordered_map(const Container& source, KeySelector keySelector, ValueSelector valueSelector)
{
	std::unordered_map<Key, Value> map;
	map.reserve(source.size());

	for (auto& item : source)
	{
		map.emplace(keySelector(item), valueSelector(item));
	}
	return map;
}