#pragma once

#include <curl/curl.h>
#include <vector>
#include <memory>
#include <string_view>
#include <utility>

#include "http_response.h"
#include "http_request.h"

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

	HttpResponse send(const HttpRequest& request) const;
};