#include <iostream>

#include "httpservice.h"
#include "httpconstants.h"

namespace
{
	curl_slist* append_headers(curl_slist* chunk, const std::vector<std::pair<std::string, std::string>>& headers)
	{
		for (auto& [key, value] : headers)
		{
			std::string header = key + ": " + value;
			chunk = curl_slist_append(chunk, header.c_str());
		}

		return chunk;
	}
}

static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	auto readBuffer = static_cast<std::string*>(userdata);
	size_t realSize = size * nmemb;
	readBuffer->append(ptr, realSize);

	return realSize;
}

HttpService::HttpService()
	: easyHandle{ curl_easy_init() }
{
	curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, write_callback);
}

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
	other.easyHandle = nullptr;
}

HttpService& HttpService::operator=(const HttpService& other)
{
	easyHandle = other.easyHandle;

	return *this;
}

HttpService& HttpService::operator=(HttpService&& other)
{
	easyHandle = other.easyHandle;

	other.easyHandle = nullptr;

	return *this;
}

std::string HttpService::get(const std::string& uri) const
{
	std::string readBuffer;

	curl_easy_setopt(easyHandle, CURLOPT_URL, uri.c_str());
	curl_easy_setopt(easyHandle, CURLOPT_CUSTOMREQUEST, HttpVerbs::GET.data());
	curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, &readBuffer);
	curl_easy_setopt(easyHandle, CURLOPT_HTTPHEADER, NULL);

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

std::string HttpService::post(const std::string& uri, const std::string& postData, const std::vector<std::pair<std::string, std::string>>& headers) const
{
	std::string readBuffer;

	curl_easy_setopt(easyHandle, CURLOPT_URL, uri.c_str());
	curl_easy_setopt(easyHandle, CURLOPT_CUSTOMREQUEST, HttpVerbs::POST.data());
	curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, &readBuffer);
	curl_easy_setopt(easyHandle, CURLOPT_POSTFIELDS, postData.c_str());

	curl_slist* chunk = append_headers(NULL, headers);
	curl_easy_setopt(easyHandle, CURLOPT_HTTPHEADER, chunk);

	CURLcode result = curl_easy_perform(easyHandle);

	curl_slist_free_all(chunk);

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
