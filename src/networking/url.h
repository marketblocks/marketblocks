#pragma once

#include <string>

class UrlQueryBuilder
{
private:
	std::string _query;
	bool started;

public:
	explicit UrlQueryBuilder();

	UrlQueryBuilder add_parameter(std::string key, std::string value);

	std::string to_string() const { return _query; }
};