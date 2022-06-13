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
				tradable_pair{ "BTC", "USDT" },
				tradable_pair{ "ETH", "USDT" }
			});
	}

	TEST(ByBitResults, Read24hStats)
	{
		execute_reader_test(
			bybit_results_test_data_path("24hr_stats.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_24h_stats,
			ohlcv_data
			{
				50005.12, 70000, 50005.12, 50008, 26.7308
			},
			assert_pair_stats_eq);
	}

	TEST(ByBitResults, ReadPrice)
	{
		execute_reader_test(
			bybit_results_test_data_path("price.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_price,
			50008.0);
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
					order_book_entry{50006.34, 0.2297, order_book_side::ASK}
				},
				std::vector<order_book_entry>
				{
					order_book_entry{50005.12, 403.0416, order_book_side::BID}
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
			{ "USDT", 10.0 }
		});
	}

	TEST(ByBitResults, ReadOpenOrders)
	{
		execute_reader_test(
			bybit_results_test_data_path("open_orders.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_open_orders,
			std::vector<order_description>
		{
			order_description{ "889788838461927936", "ETHUSDT", trade_action::BUY, 20000, 10 },
			order_description{ "888376530389004800", "BTCUSDT", trade_action::BUY, 30000, 5 }
		},
			create_vector_equal_asserter<order_description>(assert_order_description_eq));
	}

	TEST(ByBitResults, ReadClosedOrders)
	{
		execute_reader_test(
			bybit_results_test_data_path("closed_orders.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_open_orders,
			std::vector<order_description>
		{
			order_description{ "888183901021893120", "ETHUSDT", trade_action::SELL, 5000, 1.0 }
		},
			create_vector_equal_asserter<order_description>(assert_order_description_eq));
	}

	TEST(ByBitResults, ReadAddOrder)
	{
		execute_reader_test<std::string>(
			bybit_results_test_data_path("add_order.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_add_order,
			"889208273689997824");
	}

	TEST(ByBitResults, ReadCancelOrder)
	{
		execute_reader_test(
			bybit_results_test_data_path("cancel_order.json"),
			bybit_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			bybit::read_cancel_order);
	}
}