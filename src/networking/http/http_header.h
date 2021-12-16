#pragma once

#include <string>

class HttpHeader
{
private:
	std::string _key;
	std::string _value;

public:
	explicit HttpHeader(std::string key, std::string value);

	const std::string& key() const { return _key; }
	const std::string& value() const { return _value; }

	std::string to_string() const;
};