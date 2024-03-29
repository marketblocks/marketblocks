#include "exchanges/binance/binance.h"
#include "unittest/exchanges/exchange_test_common.h"
#include "unittest/exchanges/integration_tests.h"
#include "unittest/exchanges/reader_tests.h"
#include "unittest/exchanges/request_tests.h"
#include "unittest/exchanges/websocket_stream_tests.h"

namespace mb::test
{
	template<>
	tradable_pair get_testing_pair<binance_api>()
	{
		return tradable_pair{ "BTC", "USDT" };
	}

	template<>
	tradable_pair get_testing_pair_2<binance_api>()
	{
		return tradable_pair{ "ETH", "USDT" };
	}

	template<>
	std::unique_ptr<exchange> create_exchange_api<binance_api>(
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream)
	{
		return create_exchange_api<binance_api, binance_config>(std::move(httpService), websocketStream);
	}

	template<>
	std::unique_ptr<internal::binance_websocket_stream> create_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory)
	{
		std::unique_ptr<mock_exchange> mockMarketApi{ std::make_unique<mock_exchange>() };

		return std::make_unique<internal::binance_websocket_stream>(std::move(connectionFactory), std::move(mockMarketApi));
	}

	INSTANTIATE_TYPED_TEST_SUITE_P(Binance, ExchangeIntegrationTests, binance_api);
	INSTANTIATE_TYPED_TEST_SUITE_P(Binance, ExchangeReaderTests, binance_api);
	INSTANTIATE_TYPED_TEST_SUITE_P(Binance, ExchangeRequestTests, binance_api);
	INSTANTIATE_TYPED_TEST_SUITE_P(Binance, WebsocketStreamTests, internal::binance_websocket_stream);
}