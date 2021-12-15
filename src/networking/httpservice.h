#pragma once

#include <curl/curl.h>
#include <vector>
#include <memory>
#include <string_view>
#include <utility>

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
	std::string post(const std::string& uri, const std::string& postData, const std::vector<std::pair<std::string, std::string>>& headers) const;
};