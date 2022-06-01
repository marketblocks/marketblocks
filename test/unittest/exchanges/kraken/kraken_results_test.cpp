#include <gtest/gtest.h>
#include "unittest/exchanges/exchange_test_common.h"
#include "unittest/assertion_helpers.h"
#include "test_data/test_data_constants.h"

#include "exchanges/kraken/kraken_results.h"
#include "common/file/file.h"

namespace
{
	std::vector<order_description> get_expected_open_closed_orders()
	{
		return
		{
			order_description{ "OB5VMB-B4U2U-DK2WRW", "XBTUSD", trade_action::SELL, 14500.0, 0.275 },
			order_description{ "OQCLML-BW3P3-BUCMWZ", "XBTUSD", trade_action::BUY, 30010.0, 1.25 }
		};
	}
}

namespace mb::test
{
	TEST(KrakenResults, ReadSystemStatus)
	{
		execute_reader_test(kraken_results_test_data_path("system_status_online.json"), kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME), kraken::read_system_status, exchange_status::ONLINE);
		execute_reader_test(kraken_results_test_data_path("system_status_maintenance.json"), kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME), kraken::read_system_status, exchange_status::MAINTENANCE);
		execute_reader_test(kraken_results_test_data_path("system_status_cancel_only.json"), kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME), kraken::read_system_status, exchange_status::CANCEL_ONLY);
		execute_reader_test(kraken_results_test_data_path("system_status_post_only.json"), kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME), kraken::read_system_status, exchange_status::POST_ONLY);
	}

	TEST(KrakenResults, ReadTradablePairs)
	{
		execute_reader_test(
			kraken_results_test_data_path("tradable_pairs_success.json"),
			kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			kraken::read_tradable_pairs,
			std::vector<tradable_pair>
		{
			tradable_pair{ "ETH", "XBT" },
				tradable_pair{ "XBT", "USD" }
		});
	}

	TEST(KrakenResults, Read24hStats)
	{
		execute_reader_test(
			kraken_results_test_data_path("24h_stats_success.json"),
			kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			kraken::read_24h_stats,
			ohlcv_data{ 2034.26, 2056.45, 1917.23, 1940.0, 369.71101684 },
			assert_pair_stats_eq);
	}

	TEST(KrakenResults, ReadPrice)
	{
		execute_reader_test(
			kraken_results_test_data_path("24h_stats_success.json"),
			kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			kraken::read_price,
			1940.0);
	}

	TEST(KrakenResults, ReadOrderBook)
	{
		execute_reader_test(
			kraken_results_test_data_path("order_book_success.json"),
			kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			kraken::read_order_book,
			order_book_state
			{
				std::vector<order_book_entry>
				{
					order_book_entry{52523.0, 1.199},
					order_book_entry{52536.0, 0.30}
				},
				std::vector<order_book_entry>
				{
					order_book_entry{52522.9, 0.753},
					order_book_entry{52522.8, 0.006}
				}
			},
			assert_order_book_state_eq);
	}

	TEST(KrakenResults, ReadBalances)
	{
		execute_reader_test(
			kraken_results_test_data_path("balances_success.json"),
			kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			kraken::read_balances,
			unordered_string_map<double>
		{
			{ "ZUSD", 171288.6158 },
			{ "ZEUR", 504861.8946 },
			{ "ZGBP", 459567.9171 },
			{ "XXBT", 1011.19088779 },
			{ "XLTC", 2000.0 },
			{ "XETH", 818.55 }
		});
	}

	TEST(KrakenResults, ReadFee)
	{
		execute_reader_test(
			kraken_results_test_data_path("fee_success.json"),
			kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			kraken::read_fee,
			0.1);
	}

	TEST(KrakenResults, ReadOpenOrders)
	{
		execute_reader_test(
			kraken_results_test_data_path("open_orders_success.json"),
			kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			kraken::read_open_orders,
			get_expected_open_closed_orders(),
			create_vector_equal_asserter<order_description>(assert_order_description_eq));
	}

	TEST(KrakenResults, ReadClosedOrders)
	{
		execute_reader_test(
			kraken_results_test_data_path("closed_orders_success.json"),
			kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			kraken::read_closed_orders,
			get_expected_open_closed_orders(),
			create_vector_equal_asserter<order_description>(assert_order_description_eq));
	}

	TEST(KrakenResults, ReadAddOrder)
	{
		execute_reader_test<std::string>(
			kraken_results_test_data_path("add_order_success.json"),
			kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			kraken::read_add_order,
			"OUF4EM-FRGI2-MQMWZD");
	}

	TEST(KrakenResults, ReadCancelOrder)
	{
		execute_reader_test(
			kraken_results_test_data_path("cancel_order.json"),
			kraken_results_test_data_path(ERROR_RESPONSE_FILE_NAME),
			kraken::read_cancel_order);
	}
}
