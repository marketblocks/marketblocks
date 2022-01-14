#include "http_request.h"

namespace cb
{
	http_request::http_request(http_verb verb, std::string uri)
		: _verb{ verb }, _url{ std::move(uri) }, _content{}, _headers{}
	{}

	void http_request::set_content(const std::string& content)
	{
		_content = content;
	}

	void http_request::add_header(const std::string& key, const std::string& value)
	{
		_headers.emplace_back(key, value);
	}
}