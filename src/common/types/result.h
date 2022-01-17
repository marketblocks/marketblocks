#pragma once

#include <optional>
#include <string>

#include "common/exceptions/cb_exception.h"

namespace cb
{
	template<typename T>
	class result
	{
	private:
		std::optional<T> _value;
		std::string _error;

		result(T&& value)
			: _value{ std::forward<T>(value) }, _error{}
		{}

		result(std::string error)
			: _value{ std::nullopt }, _error{ std::move(error) }
		{}

	public:
		static result<T> success(T&& value)
		{
			return result<T>{ std::forward<T>(value) };
		}

		static result<T> fail(std::string error)
		{
			return result<T>{ std::move(error) };
		}

		T& value() 
		{
			if (!_value.has_value())
			{
				throw cb_exception{ "Result has no value" };
			}

			return _value.value();
		}

		std::string& error() const { return _error; }

		bool is_success() const { return _value.has_value(); }

		bool is_failure() const { return !is_success(); }
	};

}