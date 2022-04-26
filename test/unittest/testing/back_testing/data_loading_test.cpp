#include <gtest/gtest.h>

#include "unittest/mocks.h"
#include "testing/back_testing/data_loading.h"

namespace mb::test
{
	TEST(BackTestDataLoading, ReturnsDataForIncludedPairs)
	{
		back_testing_config config
		{
			0,
			1,
			"",
			{ "BTC/USD", "ETH/GBP", "BTC/GBP" }
		};

		std::vector<tradable_pair> expectedPairs
		{
			tradable_pair{ "BTC", "USD" },
			tradable_pair{ "ETH", "GBP" },
			tradable_pair{ "BTC", "GBP" }
		};

		back_testing_data data{ load_back_testing_data(config) };

		ASSERT_EQ(expectedPairs, data.tradable_pairs());
	}
}