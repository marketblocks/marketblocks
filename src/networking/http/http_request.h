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
		explicit constexpr http_request(http_verb verb, std::string uri)
			: _verb{ verb }, _url{ std::move(uri) }, _content{}, _headers{}
		{}

		friend std::ostream& operator<<(std::ostream& stream, const http_request& request);

		constexpr http_verb verb() const noexcept { return _verb; }
		constexpr const std::string& url() const noexcept { return _url; }
		constexpr const std::string& content() const noexcept { return _content; }
		constexpr const std::vector<http_header>& headers() const noexcept { return _headers; }

		constexpr void set_content(std::string_view content) noexcept
		{
			_content = content;
		}

		constexpr void add_header(std::string_view key, std::string_view value)
		{
			_headers.emplace_back(std::string{ key }, std::string{ value });
		}
	};

	std::ostream& operator<<(std::ostream& stream, const http_request& request);
}
