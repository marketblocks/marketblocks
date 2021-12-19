#include "http_request.h"

HttpRequest::HttpRequest(HttpVerb verb, std::string uri)
	: _verb{ verb }, _url{ std::move(uri) }, _content{}, _headers{}
{}

void HttpRequest::set_content(const std::string& content)
{
	_content = content;
}

void HttpRequest::add_header(const std::string& key, const std::string& value)
{
	_headers.emplace_back(key, value);
}