#pragma once

#include "networking/http/http_service.h"
#include "common/types/result.h"

namespace mb::internal
{
	template<typename Value, typename ResponseReader>
	Value send_http_request(const http_service& httpService, http_request& request, const ResponseReader& reader)
	{
		http_response response{ httpService.send(request) };

		if (response.response_code() != HttpResponseCodes::OK)
		{
			throw mb_exception{ response.message() };
		}

		result<Value> result{ reader(response.message()) };

		if (result.is_success())
		{
			return result.value();
		}

		throw mb_exception{ result.error() };
	}
}