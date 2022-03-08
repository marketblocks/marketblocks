#include <gtest/gtest.h>

#include "exchanges/coinbase/coinbase.h"
#include "unittest/mocks.h"
#include "unittest/common_matchers.h"
#include "test_data/test_data_constants.h"
#include "common/file/file.h"

namespace
{
	using namespace cb::test;
	using ::testing::Return;

	std::unique_ptr<mock_http_service> create_mock_http_service(cb::http_request expectedRequest, cb::http_response response)
	{
		std::unique_ptr<mock_http_service> mockHttpService{ std::make_unique<mock_http_service>() };
		EXPECT_CALL(*mockHttpService, send(IsHttpRequest(std::move(expectedRequest))))
			.Times(1)
			.WillOnce(Return(std::move(response)));

		return mockHttpService;
	}
}

namespace cb::test
{
	TEST(Coinbase, GetTradablePairs)
	{
		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.exchange.coinbase.com/products"
		};

		http_response response
		{
			200,
			read_file(coinbase_results_test_data_path("tradable_pairs.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service(std::move(expectedRequest), std::move(response)) };
		coinbase_api api{ std::move(mockHttpService), std::make_unique<mock_websocket_stream>() };

		api.get_tradable_pairs();
	}
}