#include <stdexcept>

#include "http_service.h"
#include "http_constants.h"

namespace
{
	curl_slist* append_headers(curl_slist* chunk, const std::vector<cb::http_header>& headers)
	{
		for (auto& header : headers)
		{
			chunk = curl_slist_append(chunk, header.to_string().c_str());
		}

		return chunk;
	}

	std::string to_string(cb::http_verb verb)
	{
		switch (verb)
		{
		case cb::http_verb::GET:
			return "GET";
		case cb::http_verb::POST:
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

namespace cb
{
	http_service::http_service()
		: easyHandle{ curl_easy_init() }
	{
		curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, write_callback);
	}

	http_service::~http_service()
	{
		curl_easy_cleanup(easyHandle);
	}

	http_service::http_service(const http_service& other)
		: easyHandle{ other.easyHandle }
	{
	}

	http_service::http_service(http_service&& other) noexcept
		: easyHandle{ other.easyHandle }
	{
		other.easyHandle = nullptr;
	}

	http_service& http_service::operator=(const http_service& other)
	{
		easyHandle = other.easyHandle;

		return *this;
	}

	http_service& http_service::operator=(http_service&& other) noexcept
	{
		easyHandle = other.easyHandle;

		other.easyHandle = nullptr;

		return *this;
	}

	http_response http_service::send(const http_request& request) const
	{
		std::string readBuffer;

		curl_easy_setopt(easyHandle, CURLOPT_URL, request.url().c_str());
		curl_easy_setopt(easyHandle, CURLOPT_CUSTOMREQUEST, to_string(request.verb()).c_str());
		curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, &readBuffer);

		curl_slist* chunk = append_headers(NULL, request.headers());
		curl_easy_setopt(easyHandle, CURLOPT_HTTPHEADER, chunk);

		CURLcode result = curl_easy_perform(easyHandle);

		if (result != CURLcode::CURLE_OK)
		{
			std::string error = curl_easy_strerror(result);
			return http_response{ 0, error };
		}

		long responseCode;
		curl_easy_getinfo(easyHandle, CURLINFO_RESPONSE_CODE, &responseCode);

		return http_response{ responseCode, readBuffer };
	}
}