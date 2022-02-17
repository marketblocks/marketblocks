#pragma once

#include <optional>
#include <variant>
#include <string>

#include "common/exceptions/cb_exception.h"

namespace cb
{
	template<typename Value>
	class result
	{
	private:
		std::variant<Value, std::string> _result;
		bool _isSuccess;

		template<typename T>
		result(T&& value, bool isSuccess)
			: _result{ std::forward<T>(value) }, _isSuccess{ isSuccess }
		{}

	public:
		static result<Value> success(Value&& value)
		{
			return result<Value>{ std::forward<Value>(value), true };
		}

		static result<Value> fail(std::string error)
		{
			return result<Value>{ std::move(error), false };
		}

		const Value& value() const
		{
			assert(is_success());
			return std::get<Value>(_result);
		}

		const std::string& error() const 
		{ 
			assert(is_failure());
			return std::get<std::string>(_result);
		}

		bool is_success() const { return _isSuccess; }

		bool is_failure() const { return !is_success(); }
	};
}