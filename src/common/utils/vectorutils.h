#pragma once

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>

namespace cb
{
	template<typename T, typename Predicate>
	std::vector<T> copy_where(const std::vector<T>& source, Predicate p)
	{
		std::vector<T> copied;
		std::copy_if(source.begin(), source.end(), std::back_inserter(copied), p);

		return copied;
	}

	template<typename T>
	bool contains(const std::vector<T>& source, T element)
	{
		return std::find(source.begin(), source.end(), element) != source.end();
	}
}