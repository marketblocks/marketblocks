#pragma once

#include <curl/curl.h>
#include <memory>
#include <string_view>

class HttpService
{
private:
	CURL* easyHandle;

public:
	HttpService();
	~HttpService();

	std::string get(const std::string_view& uri) const;
};