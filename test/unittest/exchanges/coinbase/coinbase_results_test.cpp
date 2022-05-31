#include <gtest/gtest.h>

#include "unittest/assertion_helpers.h"
#include "unittest/exchanges/results_test_common.h"
#include "test_data/test_data_constants.h"
#include "exchanges/coinbase/coinbase_results.h"

namespace mb::test
{
	TEST(CoinbaseResults, ReadTradablePairs)
	{
		execute_test(
			coinbase_results_test_data_path("tradable_pairs.json"),
			coinbase_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			coinbase::read_tradable_pairs,
			std::vector<tradable_pair>
		{
			tradable_pair{ "DOT", "USDT" },
			tradable_pair{ "BTC", "USD" }
		});
	}

	TEST(CoinbaseResults, Read24hStats)
	{
		execute_test(
			coinbase_results_test_data_path("24h_stats.json"),
			coinbase_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			coinbase::read_24h_stats,
			ohlcv_data{ 5414.18, 6441.37, 5261.69, 6250.02, 53687.76764233 },
			assert_pair_stats_eq);
	}

	TEST(CoinbaseResults, ReadPrice)
	{
		execute_test(
			coinbase_results_test_data_path("price.json"),
			coinbase_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			coinbase::read_price,
			6268.48);
	}

	TEST(CoinbaseResults, ReadOrderBook)
	{
		constexpr int depth = 2;

		execute_test(
			coinbase_results_test_data_path("order_book.json"),
			coinbase_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			[depth](std::string_view jsonResult) { return coinbase::read_order_book(jsonResult, depth); },
			order_book_state
			{ 
				std::vector<order_book_entry>
				{
					order_book_entry{ 29550.54, 0.06408441 },
					order_book_entry{ 29559.69, 0.11 }
				}, 
				std::vector<order_book_entry>
				{
					order_book_entry{ 29541.72, 0.04937732},
					order_book_entry{ 29549.16, 0.05532591 }
				}
			},
			assert_order_book_state_eq);
	}

	TEST(CoinbaseResults, ReadFee)
	{
		execute_test(
			coinbase_results_test_data_path("get_fee.json"),
			coinbase_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			coinbase::read_fee,
			0.60);
	}

	TEST(CoinbaseResults, ReadBalances)
	{
		execute_test(
			coinbase_results_test_data_path("get_balances.json"),
			coinbase_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			coinbase::read_balances,
			unordered_string_map<double>
		{
			{ "XRP", 346436.0 },
			{ "BTC", 1.354656 },
			{ "ETH", 2986657.46463 },
			{ "USD", 210480.536456 },
			{ "GBP", 464768.574 },
		});
	}

	TEST(CoinbaseResults, ReadOrders)
	{
		execute_test(
			coinbase_results_test_data_path("get_orders.json"),
			coinbase_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			coinbase::read_orders,
			std::vector<order_description>
			{
				order_description
				{
					"a9625b04-fc66-4999-a876-543c3684d702",
					"BTC-USD",
					trade_action::BUY,
					10.0,
					1.0
				}
			},
			create_vector_equal_asserter<order_description>(assert_order_description_eq));
	}

	TEST(CoinbaseResults, ReadAddOrder)
	{
		execute_test<std::string>(
			coinbase_results_test_data_path("add_order.json"),
			coinbase_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			coinbase::read_add_order,
			"a9625b04-fc66-4999-a876-543c3684d702");
	}

	TEST(CoinbaseResults, ReadCancelOrder)
	{
		execute_test(
			coinbase_results_test_data_path("cancel_order.json"), 
			coinbase_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			coinbase::read_cancel_order);
	}
}