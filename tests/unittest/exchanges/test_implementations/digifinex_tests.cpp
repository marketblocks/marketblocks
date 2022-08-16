#include "exchanges/digifinex/digifinex.h"
#include "unittest/exchanges/exchange_test_common.h"
#include "unittest/exchanges/integration_tests.h"
#include "unittest/exchanges/reader_tests.h"
#include "unittest/exchanges/request_tests.h"
#include "unittest/exchanges/websocket_stream_tests.h"

namespace mb::test
{
	template<>
	tradable_pair get_testing_pair<digifinex_api>()
	{
		return tradable_pair{ "BTC", "USDT" };
	}

	template<>
	tradable_pair get_testing_pair_2<digifinex_api>()
	{
		return tradable_pair{ "ETH", "USDT" };
	}

	template<>
	std::unique_ptr<exchange> create_exchange_api<digifinex_api>(
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream)
	{
		return create_exchange_api<digifinex_api, digifinex_config>(std::move(httpService), websocketStream);
	}

	template<>
	std::unique_ptr<internal::digifinex_websocket_stream> create_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory)
	{
		std::unique_ptr<mock_exchange> mockMarketApi{ std::make_unique<mock_exchange>() };

		ON_CALL(*mockMarketApi, get_ohlcv(_, _, _))
			.WillByDefault(Return(std::vector<ohlcv_data>{ ohlcv_data{ 1657043700, 19703.50, 19720.0, 19682.1, 19693.6, 2.38715290 } }));

		return std::make_unique<internal::digifinex_websocket_stream>(std::move(connectionFactory), std::move(mockMarketApi));
	}

	INSTANTIATE_TYPED_TEST_SUITE_P(Digifinex, ExchangeIntegrationTests, digifinex_api);
	INSTANTIATE_TYPED_TEST_SUITE_P(Digifinex, ExchangeReaderTests, digifinex_api);
	INSTANTIATE_TYPED_TEST_SUITE_P(Digifinex, ExchangeRequestTests, digifinex_api);
	INSTANTIATE_TYPED_TEST_SUITE_P(Digifinex, WebsocketStreamTests, internal::digifinex_websocket_stream);
}