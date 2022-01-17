#include <cassert>

#include "url.h"

namespace cb
{
	url_query_builder::url_query_builder()
		: _query{}, started{ false }
	{}

	url_query_builder url_query_builder::add_parameter(std::string key, std::string value)
	{
		assert(key.empty() == false);
		assert(value.empty() == false);

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
}