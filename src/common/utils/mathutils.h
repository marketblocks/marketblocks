#pragma once

#include <type_traits>
#include <concepts>
#include <string>
#include <sstream>
#include <iomanip>

namespace mb
{
	template<typename T>
	concept arithmetic = std::integral<T> || std::floating_point<T>;

	template<typename T1, typename T2>
	requires arithmetic<T1> && arithmetic<T2>
	constexpr double calculate_percentage_diff(T1 a, T2 b) noexcept
	{
		return (b - a) * 100.0 / a;
	}

	template<typename T1, typename T2, typename T3>
	requires arithmetic<T1> && arithmetic<T2> && arithmetic<T3>
	constexpr double calculate_percentage_proportion(T1 a, T2 b, T3 t) noexcept
	{
		return (t - a) * 100.0 / (b - a);
	}

	bool double_equal(double a, double b);
	bool double_less_than(double a, double b);
	bool double_greater_than(double a, double b);

	template<typename T>
	requires std::floating_point<T>
	constexpr std::string to_string(T number, int precision)
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(precision) << number;
		return stream.str();
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
		
		constexpr void reset()
		{
			_mean = T{};
			_valueSum = T{};
			count = 0;
		}
	};
}