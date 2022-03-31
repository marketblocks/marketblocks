#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iterator>

namespace mb
{
	template<typename Key, typename Value, typename Container, typename KeySelector, typename ValueSelector>
	constexpr std::unordered_map<Key, Value> to_unordered_map(const Container& source, KeySelector keySelector, ValueSelector valueSelector)
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
	constexpr std::vector<T> to_vector(const Container& source)
	{
		return to_vector<T>(source, [](const T& item) { return item; });
	}

	template<typename T, typename Container, typename Selector>
	constexpr std::vector<T> to_vector(const Container& source, Selector selector)
	{
		std::vector<T> vector;
		vector.reserve(source.size());

		for (auto& item : source)
		{
			vector.emplace_back(selector(item));
		}

		return vector;
	}

	template<typename T, typename Container, typename Predicate>
	constexpr T copy_where(const Container& source, Predicate p)
	{
		T copied;
		std::copy_if(source.begin(), source.end(), std::back_inserter(copied), p);

		return copied;
	}

	template<typename T, typename Container>
	constexpr bool contains(const Container& source, T element)
	{
		return std::find(source.begin(), source.end(), element) != source.end();
	}
}