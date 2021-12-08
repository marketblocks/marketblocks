#include <iostream>

#include "httpservice.h"
#include "httpconstants.h"

static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	auto readBuffer = static_cast<std::string*>(userdata);
	size_t realSize = size * nmemb;
	readBuffer->append(ptr, realSize);

	return realSize;
}

HttpService::HttpService()
	: easyHandle{ curl_easy_init() }
{}

HttpService::~HttpService()
{
	curl_easy_cleanup(easyHandle);
}

HttpService::HttpService(const HttpService& other)
	: easyHandle { other.easyHandle }
{
}

HttpService::HttpService(HttpService&& other)
	: easyHandle { other.easyHandle }
{
	other.easyHandle = 0;
}

HttpService& HttpService::operator=(const HttpService& other)
{
	easyHandle = other.easyHandle;

	return *this;
}

HttpService& HttpService::operator=(HttpService&& other)
{
	easyHandle = other.easyHandle;

	other.easyHandle = 0;

	return *this;
}

std::string HttpService::get(const std::string& uri) const
{
	std::string readBuffer;

	curl_easy_setopt(easyHandle, CURLOPT_URL, uri.c_str());
	curl_easy_setopt(easyHandle, CURLOPT_CUSTOMREQUEST, HttpVerbs::GET.data());
	curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, &write_callback);
	curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, &readBuffer);

	CURLcode result = curl_easy_perform(easyHandle);

	if (result != CURLcode::CURLE_OK)
	{
		std::cerr << curl_easy_strerror(result) << std::endl;
	}
	
	long responseCode;
	curl_easy_getinfo(easyHandle, CURLINFO_RESPONSE_CODE, &responseCode);

	if (responseCode != HttpResponseCodes::OK)
	{
		std::cerr << "Http Error: " << responseCode << std::endl;
	}

	return readBuffer;
}
