#include "url.h"

UrlQueryBuilder::UrlQueryBuilder()
	: _query{}, started{ false }
{}

UrlQueryBuilder UrlQueryBuilder::add_parameter(std::string key, std::string value)
{
	if (started)
	{
		_query += "&";
	}
	else
	{
		started = true;
	}

	_query += std::move(key) + "=" + std::move(value);

	return *this;
}