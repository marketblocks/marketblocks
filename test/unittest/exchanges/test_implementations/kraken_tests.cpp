#include "exchanges/kraken/kraken.h"
#include "unittest/exchanges/exchange_test_common.h"
#include "unittest/exchanges/integration_tests.h"
#include "unittest/exchanges/reader_tests.h"
#include "unittest/exchanges/request_tests.h"
#include "unittest/exchanges/websocket_stream_tests.h"

namespace mb::test
{
	template<>
	tradable_pair get_testing_pair<kraken_api>()
	{
		return tradable_pair{ "BTC", "USD" };
	}

	template<>
	std::unique_ptr<exchange> create_exchange_api<kraken_api>(
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream)
	{
		return create_exchange_api<kraken_api, kraken_config>(std::move(httpService), websocketStream);
	}

	INSTANTIATE_TYPED_TEST_SUITE_P(Kraken, ExchangeIntegrationTests, kraken_api);
	INSTANTIATE_TYPED_TEST_SUITE_P(Kraken, ExchangeReaderTests, kraken_api);
	INSTANTIATE_TYPED_TEST_SUITE_P(Kraken, ExchangeRequestTests, kraken_api);
	INSTANTIATE_TYPED_TEST_SUITE_P(Kraken, WebsocketStreamTests, internal::kraken_websocket_stream);
}