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

void append_query(std::string& url, const std::string& _query)
{
	if (!_query.empty())
	{
		url += "?" + _query;
	}
}

std::string build_url(const std::string& baseUrl, const std::string& path, const std::string& _query)
{
	std::string url = baseUrl + path;

	append_query(url, _query);

	return url;
}