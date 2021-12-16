#include <stdexcept>

#include "http_service.h"
#include "http_constants.h"

namespace
{
	curl_slist* append_headers(curl_slist* chunk, const std::vector<HttpHeader>& headers)
	{
		for (auto& header : headers)
		{
			chunk = curl_slist_append(chunk, header.to_string().c_str());
		}

		return chunk;
	}

	std::string to_string(HttpVerb verb)
	{
		switch (verb)
		{
		case HttpVerb::GET:
			return "GET";
		case HttpVerb::POST:
			return "POST";
		}

		throw std::invalid_argument("HTTP verb not recognized");
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

HttpResponse HttpService::send(const HttpRequest& request) const
{
	std::string readBuffer;

	curl_easy_setopt(easyHandle, CURLOPT_URL, request.uri().value().c_str());
	curl_easy_setopt(easyHandle, CURLOPT_CUSTOMREQUEST, to_string(request.verb()).c_str());
	curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, &readBuffer);

	curl_slist* chunk = append_headers(NULL, request.headers());
	curl_easy_setopt(easyHandle, CURLOPT_HTTPHEADER, chunk);

	CURLcode result = curl_easy_perform(easyHandle);

	if (result != CURLcode::CURLE_OK)
	{
		std::string error = curl_easy_strerror(result);
		return HttpResponse{ 0, error };
	}

	long responseCode;
	curl_easy_getinfo(easyHandle, CURLINFO_RESPONSE_CODE, &responseCode);

	return HttpResponse{ responseCode, readBuffer };
}
