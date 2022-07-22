#pragma once

#include <string>

namespace mb
{
	class http_header
	{
	private:
		std::string _key;
		std::string _value;

	public:
		http_header(std::string key, std::string value)
			: _key{ std::move(key) }, _value{ std::move(value) }
		{}

		const std::string& key() const noexcept { return _key; }
		const std::string& value() const noexcept { return _value; }

		std::string to_string() const
		{
			return _key + ": " + _value;
		}
	};
}