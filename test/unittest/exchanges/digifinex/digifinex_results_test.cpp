#include <gtest/gtest.h>

#include "unittest/assertion_helpers.h"
#include "unittest/exchanges/exchange_test_common.h"
#include "test_data/test_data_constants.h"
#include "exchanges/digifinex/digifinex_results.h"

namespace mb::test
{
	TEST(DigifinexResults, ReadSystemStatus)
	{
		execute_reader_test(
			digifinex_results_test_data_path("ping.json"),
			digifinex_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			digifinex::read_system_status,
			exchange_status::ONLINE);
	}

	TEST(DigifinexResults, ReadTradablePairs)
	{
		execute_reader_test(
			digifinex_results_test_data_path("tradable_pairs.json"),
			digifinex_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			digifinex::read_tradable_pairs,
			std::vector<tradable_pair>
			{
				tradable_pair{ "BTC", "USD" },
				tradable_pair{ "ETH", "USD" },
				tradable_pair{ "DFT", "GBP" }
			});
	}

	TEST(DigifinexResults, Read24hStats)
	{
		execute_reader_test(
			digifinex_results_test_data_path("24h_stats.json"),
			digifinex_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			digifinex::read_24h_stats,
			ohlcv_data
			{
				9307.96, 9349.99, 9305.86, 9342.7, 621.4565
			},
			assert_pair_stats_eq);
	}

	TEST(DigifinexResults, ReadPrice)
	{
		execute_reader_test(
			digifinex_results_test_data_path("get_price.json"),
			digifinex_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			digifinex::read_price,
			9592.22);
	}

	TEST(DigifinexResults, ReadOrderBook)
	{
		execute_reader_test(
			digifinex_results_test_data_path("order_book.json"),
			digifinex_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			digifinex::read_order_book,
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

	TEST(DigifinexResults, ReadBalances)
	{
		execute_reader_test(
			digifinex_results_test_data_path("get_balances.json"),
			digifinex_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			digifinex::read_balances,
			unordered_string_map<double>
		{
			{ "BTC", 4723846.89208129 }
		});
	}

	TEST(DigifinexResults, ReadOpenOrders)
	{
		execute_reader_test(
			digifinex_results_test_data_path("open_orders.json"),
			digifinex_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			digifinex::read_open_orders,
			std::vector<order_description>
			{
				order_description{ "dd3164b333a4afa9d5730bb87f6db8b3", "BTC_USDT", trade_action::BUY, 0.1, 1.0 }
			},
			create_vector_equal_asserter<order_description>(assert_order_description_eq));
	}

	TEST(DigifinexResults, ReadClosedOrders)
	{
		execute_reader_test(
			digifinex_results_test_data_path("all_orders.json"),
			digifinex_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			digifinex::read_open_orders,
			std::vector<order_description>
		{
			order_description{ "198361cecdc65f9c8c9bb2fa68faec40", "ETH_USD", trade_action::SELL, 0.7, 2.0 }
		},
			create_vector_equal_asserter<order_description>(assert_order_description_eq));
	}

	TEST(DigifinexResults, ReadAddOrder)
	{
		execute_reader_test<std::string>(
			digifinex_results_test_data_path("add_order.json"),
			digifinex_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			digifinex::read_add_order,
			"198361cecdc65f9c8c9bb2fa68faec40");
	}

	TEST(DigifinexResults, ReadCancelOrderSuccess)
	{
		execute_reader_test(
			digifinex_results_test_data_path("cancel_order_success.json"),
			digifinex_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			digifinex::read_cancel_order);
	}

	TEST(DigifinexResults, ReadCancelOrderFail)
	{
		result<void> result = execute_reader(
			digifinex_results_test_data_path("cancel_order_failure.json"),
			digifinex::read_cancel_order);

		EXPECT_TRUE(result.is_failure());
	}
}