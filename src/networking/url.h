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
		url_query_builder add_parameter(std::string_view key, std::string_view value);
		
		std::string to_string() const noexcept { return _query; }
	};

	void append_query(std::string& url, std::string_view query);
	void append_path(std::string& url, std::string_view path);
	std::string build_url(std::string_view baseUrl, std::string_view path, std::string_view _query = "");

	template<typename PathComponents>
	std::string build_url_path(const PathComponents& pathComponents)
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