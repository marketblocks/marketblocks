#include <gtest/gtest.h>

#include "unittest/assertion_helpers.h"
#include "unittest/exchanges/results_test_common.h"
#include "test_data/test_data_constants.h"
#include "exchanges/bybit/bybit_results.h"

namespace mb::test
{
	TEST(ByBitResults, ReadSystemStatus)
	{
	}

	TEST(ByBitResults, ReadTradablePairs)
	{
		execute_test(
			bybit_results_test_data_path("tradable_pairs.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_tradable_pairs,
			std::vector<tradable_pair>
			{
				tradable_pair{ "BTC", "USD" },
				tradable_pair{ "EOS", "USD" },
				tradable_pair{ "BTC", "USDT" }
			});
	}

	TEST(ByBitResults, Read24hStats)
	{
	}

	TEST(ByBitResults, ReadPrice)
	{
	}

	TEST(ByBitResults, ReadOrderBook)
	{
	}

	TEST(ByBitResults, ReadBalances)
	{
		execute_test(
			bybit_results_test_data_path("get_balances.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_balances,
			unordered_string_map<double>
		{
			{ "BTC", 999.99987471 }
		});
	}

	TEST(ByBitResults, ReadFee)
	{
	}

	TEST(ByBitResults, ReadOpenOrders)
	{
	}

	TEST(ByBitResults, ReadClosedOrders)
	{
	}

	TEST(ByBitResults, ReadAddOrder)
	{
	}

	TEST(ByBitResults, ReadCancelOrder)
	{
	}
}