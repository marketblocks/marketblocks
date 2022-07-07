#include "unittest/exchanges/exchange_test_common.h"
#include "unittest/exchanges/integration_tests.h"
#include "unittest/exchanges/reader_tests.h"
#include "unittest/exchanges/request_tests.h"
#include "exchanges/bybit/bybit.h"

namespace mb::test
{
	template<>
	tradable_pair get_testing_pair<bybit_api>()
	{
		return tradable_pair{ "BTC", "USDT" };
	}

	template<>
	std::unique_ptr<exchange> create_exchange_api<bybit_api>(
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream)
	{
		return create_exchange_api<bybit_api, bybit_config>(std::move(httpService), websocketStream);
	}

	INSTANTIATE_TYPED_TEST_SUITE_P(ByBit, ExchangeIntegrationTests, bybit_api);
	INSTANTIATE_TYPED_TEST_SUITE_P(ByBit, ExchangeReaderTests, bybit_api);
	INSTANTIATE_TYPED_TEST_SUITE_P(ByBit, ExchangeRequestTests, bybit_api);
}