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
		static constexpr int VALUE_INDEX = 0;
		static constexpr int ERROR_INDEX = 1;

		using result_variant = std::variant<Value, std::string>;
		
		result_variant _result;
		bool _isSuccess;

		result(result_variant value, bool isSuccess)
			: _result{ std::move(value) }, _isSuccess{ isSuccess }
		{}

	public:
		static result<Value> success(Value&& value)
		{
			return result<Value>{ result_variant{ std::in_place_index<VALUE_INDEX>, std::forward<Value>(value) }, true };
		}

		static result<Value> fail(std::string error)
		{
			return result<Value>{ result_variant{ std::in_place_index<ERROR_INDEX>, std::move(error) }, false };
		}

		const Value& value() const
		{
			assert(is_success());
			return std::get<VALUE_INDEX>(_result);
		}

		const std::string& error() const 
		{ 
			assert(is_failure());
			return std::get<ERROR_INDEX>(_result);
		}

		bool is_success() const { return _isSuccess; }

		bool is_failure() const { return !is_success(); }
	};
}