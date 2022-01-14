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
		explicit http_header(std::string key, std::string value);

		const std::string& key() const { return _key; }
		const std::string& value() const { return _value; }

		std::string to_string() const;
	};
}