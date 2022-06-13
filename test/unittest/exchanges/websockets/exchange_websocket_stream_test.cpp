#include <gtest/gtest.h>

#include "exchanges/websockets/exchange_websocket_stream.h"
#include "unittest/mocks.h"

namespace mb::test
{
	using ::testing::_;

	TEST(ExchangeWebsocketStream, CreatesConnectionOnConstruction)
	{
		auto mockConnectionFactory = std::make_unique<mock_websocket_connection_factory>();
		EXPECT_CALL(*mockConnectionFactory, create_connection(_)).Times(1);

		mock_exchange_websocket_stream testWebsocketStream{ "", "", std::move(mockConnectionFactory) };
	}
}