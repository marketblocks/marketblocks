#include "unittest/exchanges/exchange_test_common.h"
#include "unittest/exchanges/integration_tests.h"
#include "unittest/exchanges/reader_tests.h"
#include "exchanges/coinbase/coinbase.h"

namespace mb::test
{
	template<>
	tradable_pair get_testing_pair<coinbase_api>()
	{
		return tradable_pair{ "BTC", "USD" };
	}

	template<>
	std::unique_ptr<exchange> create_exchange_api<coinbase_api>(
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream)
	{
		return create_exchange_api<coinbase_api, coinbase_config>(std::move(httpService), websocketStream);
	}

	INSTANTIATE_TYPED_TEST_SUITE_P(Coinbase, ExchangeIntegrationTests, coinbase_api);
	INSTANTIATE_TYPED_TEST_SUITE_P(Coinbase, ExchangeReaderTests, coinbase_api);
}