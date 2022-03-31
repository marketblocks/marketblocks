#pragma once

#include <type_traits>

namespace mb
{
	template<typename T>
	concept arithmetic = std::integral<T> || std::floating_point<T>;

	template<typename T1, typename T2>
	requires arithmetic<T1> && arithmetic<T2>
	constexpr double calculate_percentage_diff(T1 a, T2 b) noexcept
	{
		return (b - a) * 100 / static_cast<double>(a);
	}
}