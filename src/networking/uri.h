#pragma once

#include <string>

class Uri
{
private:
	std::string _value;

public:
	Uri(std::string value);

	const std::string& value() const { return _value; }
};