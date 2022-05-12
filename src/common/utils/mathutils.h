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

	template<typename T>
	class mean_calculator
	{
	private:
		T _mean;
		T _valueSum;
		int count;

	public:
		constexpr void add_value(T value)
		{
			_valueSum += value;
			++count;
			_mean = _valueSum / count;
		}

		constexpr T mean() const noexcept { return _mean; }
	};
}