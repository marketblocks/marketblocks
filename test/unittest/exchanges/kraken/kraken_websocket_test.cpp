#include <gtest/gtest.h>

#include "exchanges/kraken/kraken_websocket.h"

namespace cb::test
{
	using namespace cb::internal;

	TEST(KrakenWebsocket, CreatesCorrectOrderBookSubscriptionMessage)
	{
		kraken_websocket_stream krakenWebsocket{};
		std::vector<tradable_pair> tradablePairs
		{
			tradable_pair{ "BTC", "GBP" },
			tradable_pair{ "ETH", "USD" }
		};
		std::string expectedMessage{ "{\"event\":\"subscribe\",\"pair\":[\"BTC/GBP\",\"ETH/USD\"],\"subscription\":{\"name\":\"book\"}}" };

		std::string actualMessage{ krakenWebsocket.get_order_book_subscription_message(tradablePairs) };
		ASSERT_EQ(expectedMessage, actualMessage);
	}
}