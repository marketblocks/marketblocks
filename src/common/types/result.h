#pragma once

#include <optional>
#include <variant>
#include <string>
#include <cassert>

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

		constexpr result(result_variant value, bool isSuccess)
			: _result{ std::move(value) }, _isSuccess{ isSuccess }
		{}

	public:
		static constexpr result<Value> success(Value&& value) noexcept
		{
			return result<Value>{ result_variant{ std::in_place_index<VALUE_INDEX>, std::forward<Value>(value) }, true };
		}

		static constexpr result<Value> fail(std::string error) noexcept
		{
			return result<Value>{ result_variant{ std::in_place_index<ERROR_INDEX>, std::move(error) }, false };
		}

		constexpr const Value& value() const
		{
			assert(is_success());
			return std::get<VALUE_INDEX>(_result);
		}

		constexpr const std::string& error() const
		{ 
			assert(is_failure());
			return std::get<ERROR_INDEX>(_result);
		}

		constexpr bool is_success() const noexcept { return _isSuccess; }

		constexpr bool is_failure() const noexcept { return !is_success(); }
	};

	template<>
	class result<void>
	{
	private:
		std::optional<std::string> _error;

		explicit constexpr result(std::string error)
			: _error{ std::move(error) }
		{}

		constexpr result()
			: _error{ std::nullopt }
		{}

	public:
		static constexpr result<void> success() noexcept
		{
			return result<void>{};
		}

		static constexpr result<void> fail(std::string error) noexcept
		{
			return result<void>{ std::move(error) };
		}

		constexpr void value() const noexcept {}

		constexpr const std::string& error() const
		{
			assert(is_failure());
			return _error.value();
		}

		constexpr bool is_failure() const noexcept { return _error.has_value(); }

		constexpr bool is_success() const noexcept { return !is_failure(); }
	};
}