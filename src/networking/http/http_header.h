#pragma once

#include <string>

namespace cb
{
	class http_header
	{
	private:
		std::string _key;
		std::string _value;

	public:
		explicit constexpr http_header(std::string key, std::string value)
			: _key{ std::move(key) }, _value{ std::move(value) }
		{}

		constexpr const std::string& key() const noexcept { return _key; }
		constexpr const std::string& value() const noexcept { return _value; }

		constexpr std::string to_string() const
		{
			return _key + ": " + _value;
		}
	};
}