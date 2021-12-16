#pragma once

#include <utility>
#include <vector>
#include "networking/uri.h"
#include "http_constants.h"
#include "http_header.h"

class HttpRequest
{
private:
	HttpVerb _verb;
	Uri _uri;
	std::string _content;
	std::vector<HttpHeader> _headers;

public:
	explicit HttpRequest(HttpVerb verb, Uri uri);

	HttpVerb verb() const { return _verb; }
	const Uri& uri() const { return _uri; }
	const std::string& content() const { return _content; }
	const std::vector<HttpHeader>& headers() const { return _headers; }

	void set_content(const std::string& content);
	void add_header(const std::string& key, const std::string& value);
};