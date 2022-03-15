#pragma once

#include <gmock/gmock.h>
#include <vector>

#include "exchanges/exchange.h"
#include "networking/http/http_service.h"

namespace cb::test
{
	class mock_http_service : public http_service
	{
	public:
		MOCK_METHOD(http_response, send, (const http_request& request), (const, override));
	};

	class mock_websocket_stream : public websocket_stream
	{
	public:
		mock_websocket_stream()
			: websocket_stream{ nullptr, nullptr }
		{}
	};

}
