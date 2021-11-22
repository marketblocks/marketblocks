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