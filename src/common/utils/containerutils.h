#include <unordered_map>

namespace cb
{
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

	template<typename T, typename Container>
	std::vector<T> to_vector(const Container& source)
	{
		return to_vector<T>(source, [](const T& item) { return item; });
	}

	template<typename T, typename Container, typename Selector>
	std::vector<T> to_vector(const Container& source, Selector selector)
	{
		std::vector<T> vector;
		vector.reserve(source.size());

		for (auto& item : source)
		{
			vector.emplace_back(selector(item));
		}

		return vector;
	}
}