#pragma once

#include <utility>
#include <vector>
#include <iostream>

#include "http_constants.h"
#include "http_header.h"

namespace mb
{
	class http_request
	{
	private:
		http_verb _verb;
		std::string _url;
		std::string _content;
		std::vector<http_header> _headers;

	public:
		http_request(http_verb verb, std::string uri);

		friend std::ostream& operator<<(std::ostream& stream, const http_request& request);

		http_verb verb() const noexcept { return _verb; }
		const std::string& url() const noexcept { return _url; }
		const std::string& content() const noexcept { return _content; }
		const std::vector<http_header>& headers() const noexcept { return _headers; }

		void set_content(std::string_view content) noexcept	{ _content = content; }

		void add_header(std::string_view key, std::string_view value);
	};

	std::ostream& operator<<(std::ostream& stream, const http_request& request);
}
