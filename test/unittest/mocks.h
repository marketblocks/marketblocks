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
			: websocket_stream{ nullptr }
		{}

		MOCK_METHOD(std::string, stream_url, (), (const, override));
		MOCK_METHOD(void, on_open, (), (override));
		MOCK_METHOD(void, on_close, (std::string_view), (override));
		MOCK_METHOD(void, on_fail, (std::string_view), (override));
		MOCK_METHOD(void, on_message, (std::string_view), (override));

		MOCK_METHOD(void, subscribe_order_book, (const std::vector<cb::tradable_pair>& tradablePairs), (override));
	};

	class mock_websocket_client : public websocket_client
	{

	};
}
