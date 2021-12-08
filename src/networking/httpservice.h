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

	HttpService(const HttpService& other);
	HttpService(HttpService&& other);

	HttpService& operator=(const HttpService& other);
	HttpService& operator=(HttpService&& other);

	std::string get(const std::string& uri) const;
};