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
		explicit url_query_builder();

		url_query_builder add_parameter(std::string key, std::string value);

		std::string to_string() const { return _query; }
	};

	void append_query(std::string& url, const std::string& _query);

	std::string build_url(const std::string& baseUrl, const std::string& path, const std::string& _query);
}