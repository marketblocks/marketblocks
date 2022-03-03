#pragma once

#include <string>

namespace cb
{
	class url_query_builder
	{
	private:
		std::string _query;
		bool started;

	public:
		explicit constexpr url_query_builder()
			: _query{}, started{ false }
		{}

		constexpr url_query_builder add_parameter(std::string_view key, std::string_view value)
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

			_query.append(key);
			_query.append("=");
			_query.append(value);

			return *this;
		}

		constexpr std::string to_string() const noexcept { return _query; }
	};

	constexpr void append_query(std::string& url, std::string_view _query)
	{
		if (!_query.empty())
		{
			url.append("?");
			url.append(_query);
		}
	}

	constexpr std::string build_url(std::string_view baseUrl, std::string_view path, std::string_view _query)
	{
		std::string url{ baseUrl };
		
		url.append(path);

		append_query(url, _query);

		return url;
	}
}