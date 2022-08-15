#pragma once

#include <string>
#include <sstream>
#include <iomanip>

namespace mb
{
	template<typename T1, typename T2>
	constexpr double calculate_percentage_diff(T1 a, T2 b) noexcept
	{
		return (b - a) * 100.0 / a;
	}

	template<typename T1, typename T2, typename T3>
	constexpr double calculate_percentage_proportion(T1 a, T2 b, T3 t) noexcept
	{
		return (t - a) * 100.0 / (b - a);
	}

	template<typename T>
	constexpr T round(T value, int dp)
	{
		T shift{ std::pow(static_cast<T>(10.0), dp) };
		double rounded{ std::round(value * shift) };
		return rounded / shift;
	}

	bool double_equal(double a, double b);
	bool double_less_than(double a, double b);
	bool double_greater_than(double a, double b);

	template<typename T>
	std::string to_string(T number, int precision)
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