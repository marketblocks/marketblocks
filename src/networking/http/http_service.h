#pragma once

#include <curl/curl.h>
#include <vector>
#include <memory>
#include <string_view>
#include <utility>

#include "http_response.h"
#include "http_request.h"

namespace cb
{
	class http_service
	{
	private:
		CURL* easyHandle;

	public:
		http_service();
		~http_service();

		http_service(const http_service& other);
		http_service(http_service&& other);

		http_service& operator=(const http_service& other);
		http_service& operator=(http_service&& other);

		http_response send(const http_request& request) const;
	};
}