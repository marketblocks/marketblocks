#pragma once

#include <utility>
#include <vector>
#include "http_constants.h"
#include "http_header.h"

class HttpRequest
{
private:
	HttpVerb _verb;
	std::string _url;
	std::string _content;
	std::vector<HttpHeader> _headers;

public:
	explicit HttpRequest(HttpVerb verb, std::string url);

	HttpVerb verb() const { return _verb; }
	const std::string& url() const { return _url; }
	const std::string& content() const { return _content; }
	const std::vector<HttpHeader>& headers() const { return _headers; }

	void set_content(const std::string& content);
	void add_header(const std::string& key, const std::string& value);
};