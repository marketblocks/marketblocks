#include <stdexcept>

#include "http_service.h"
#include "http_constants.h"
#include "http_error.h"

namespace
{
	using namespace mb;

	size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
	{
		auto readBuffer = static_cast<std::string*>(userdata);
		size_t realSize = size * nmemb;
		readBuffer->append(ptr, realSize);

		return realSize;
	}

	void throw_if_error(CURLcode result)
	{
		if (result != CURLcode::CURLE_OK)
		{
			std::string error = curl_easy_strerror(result);
			throw http_error{ std::move(error) };
		}
	}

	template<typename... Args>
	void set_option(CURL* handle, CURLoption option, Args&&... args)
	{
		CURLcode result = curl_easy_setopt(handle, option, std::forward<Args>(args)...);
		throw_if_error(result);
	}

	curl_slist* append_headers(curl_slist* chunk, const std::vector<http_header>& headers)
	{
		for (auto& header : headers)
		{
			chunk = curl_slist_append(chunk, header.to_string().c_str());
		}

		return chunk;
	}
}

namespace mb
{
	http_service::http_service()
		: easyHandle{ curl_easy_init() }
	{
		if (!easyHandle)
		{
			throw http_error{ "Could not create HTTP service" };
		}

		set_option(easyHandle, CURLOPT_WRITEFUNCTION, write_callback);
		set_option(easyHandle, CURLOPT_TIMEOUT_MS, _timeout);
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

		set_option(easyHandle, CURLOPT_URL, request.url().c_str());
		set_option(easyHandle, CURLOPT_CUSTOMREQUEST, to_string(request.verb()).c_str());
		set_option(easyHandle, CURLOPT_WRITEDATA, &readBuffer);
		set_option(easyHandle, CURLOPT_POSTFIELDS, request.content().c_str());

		curl_slist* chunk = append_headers(NULL, request.headers());
		set_option(easyHandle, CURLOPT_HTTPHEADER, chunk);

		CURLcode result = curl_easy_perform(easyHandle);

		curl_slist_free_all(chunk);

		throw_if_error(result);

		long responseCode;
		curl_easy_getinfo(easyHandle, CURLINFO_RESPONSE_CODE, &responseCode);

		return http_response{ responseCode, readBuffer };
	}
}