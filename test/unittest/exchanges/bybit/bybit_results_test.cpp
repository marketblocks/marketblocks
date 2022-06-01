#include <gtest/gtest.h>

#include "unittest/assertion_helpers.h"
#include "unittest/exchanges/exchange_test_common.h"
#include "test_data/test_data_constants.h"
#include "exchanges/bybit/bybit_results.h"

namespace mb::test
{
	TEST(ByBitResults, ReadSystemStatus)
	{
		execute_reader_test(
			bybit_results_test_data_path("server_time.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_system_status,
			exchange_status::ONLINE);
	}

	TEST(ByBitResults, ReadTradablePairs)
	{
		execute_reader_test(
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
		execute_reader_test(
			bybit_results_test_data_path("symbol_stats.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_24h_stats,
			ohlcv_data
			{
				7163.00, 7267.50, 7067.00, 7230.00, 78053288
			},
			assert_pair_stats_eq);
	}

	TEST(ByBitResults, ReadPrice)
	{
		execute_reader_test(
			bybit_results_test_data_path("symbol_stats.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_price,
			7230.00);
	}

	TEST(ByBitResults, ReadOrderBook)
	{
		execute_reader_test(
			bybit_results_test_data_path("order_book.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_order_book,
			order_book_state
			{
				std::vector<order_book_entry>
				{
					order_book_entry{9563.45, 0.6312},
					order_book_entry{9563.34, 0.0087}
				},
				std::vector<order_book_entry>
				{
					order_book_entry{9559.45, 1.3766},
					order_book_entry{9559.04, 0.0127}
				}
			},
			assert_order_book_state_eq);
	}

	TEST(ByBitResults, ReadBalances)
	{
		execute_reader_test(
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
		execute_reader_test(
			bybit_results_test_data_path("fee.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_fee,
			0.1);
	}

	TEST(ByBitResults, ReadOpenOrders)
	{
		execute_reader_test(
			bybit_results_test_data_path("open_orders.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_open_orders,
			std::vector<order_description>
		{
			order_description{ "dd3164b333a4afa9d5730bb87f6db8b3", "BTC_USDT", trade_action::BUY, 0.1, 1.0 }
		},
			create_vector_equal_asserter<order_description>(assert_order_description_eq));
	}

	TEST(ByBitResults, ReadClosedOrders)
	{
		execute_reader_test(
			bybit_results_test_data_path("all_orders.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_open_orders,
			std::vector<order_description>
		{
			order_description{ "198361cecdc65f9c8c9bb2fa68faec40", "ETH_USD", trade_action::SELL, 0.7, 2.0 }
		},
			create_vector_equal_asserter<order_description>(assert_order_description_eq));
	}

	TEST(ByBitResults, ReadAddOrder)
	{
		execute_reader_test<std::string>(
			bybit_results_test_data_path("add_order.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_add_order,
			"335fd977-e5a5-4781-b6d0-c772d5bfb95b");
	}

	TEST(ByBitResults, ReadCancelOrder)
	{
		execute_reader_test(
			bybit_results_test_data_path("cancel_order.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_cancel_order);
	}
}