#pragma once

#include <utility>
#include <vector>
#include "http_constants.h"
#include "http_header.h"

namespace cb
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

		constexpr http_verb verb() const noexcept { return _verb; }
		constexpr const std::string& url() const noexcept { return _url; }
		constexpr const std::string& content() const noexcept { return _content; }
		constexpr const std::vector<http_header>& headers() const noexcept { return _headers; }

		constexpr void set_content(std::string content) noexcept
		{
			_content = std::move(content);
		}

		constexpr void add_header(std::string key, std::string value)
		{
			_headers.emplace_back(std::move(key), std::move(value));
		}
	};
}
