#pragma once

#include <string>

namespace mb
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

	constexpr void append_query(std::string& url, std::string_view query)
	{
		if (!query.empty())
		{
			url.append("?");
			url.append(query);
		}
	}

	constexpr void append_path(std::string& url, std::string_view path)
	{
		if (!path.empty())
		{
			if (url.back() != '/' && path.front() != '/')
			{
				url.append("/");
			}

			url.append(path);
		}
	}

	constexpr std::string build_url(std::string_view baseUrl, std::string_view path, std::string_view _query = "")
	{
		std::string url{ baseUrl };
		
		append_path(url, path);
		append_query(url, _query);

		return url;
	}

	template<typename PathComponents>
	constexpr std::string build_url_path(const PathComponents& pathComponents)
	{
		std::string path{"/"};

		for (auto& component : pathComponents)
		{
			path.append(component);
			path.append("/");
		}

		path.pop_back();

		return path;
	}
}