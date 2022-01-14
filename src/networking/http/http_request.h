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
		explicit http_request(http_verb verb, std::string url);

		http_verb verb() const { return _verb; }
		const std::string& url() const { return _url; }
		const std::string& content() const { return _content; }
		const std::vector<http_header>& headers() const { return _headers; }

		void set_content(const std::string& content);
		void add_header(const std::string& key, const std::string& value);
	};
}
