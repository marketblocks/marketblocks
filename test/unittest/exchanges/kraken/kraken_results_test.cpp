#pragma once

#include <gtest/gtest.h>
#include "unittest/assertion_helpers.h"
#include "test_data/test_data_constants.h"

#include "exchanges/kraken/kraken_results.h"
#include "common/file/file.h"

namespace
{
	using namespace mb;
	using namespace mb::test;
	
	static const std::string ERROR_MESSAGE = "This is an error";
	static const std::string ERROR_RESPONSE_FILE_NAME = "error_response.json";

	template<typename ResultReader>
	auto execute_reader(const std::string& dataFileName, ResultReader reader)
	{
		std::filesystem::path dataPath{ kraken_results_test_data_path(dataFileName) };
		std::string json = read_file(dataPath);

		return reader(json);
	}

	template<typename T, typename ResultReader, typename ValueAsserter>
	void execute_test(const std::string& testDataFileName, const ResultReader& reader, T expectedValue, const ValueAsserter& valueAsserter)
	{
		assert_result_equal(
			execute_reader(testDataFileName, reader),
			result<T>::success(std::move(expectedValue)),
			valueAsserter);

		assert_result_equal(
			execute_reader(ERROR_RESPONSE_FILE_NAME, reader),
			result<T>::fail(ERROR_MESSAGE),
			no_assert<T>);
	}

	template<typename T, typename ResultReader>
	void execute_test(const std::string& testDataFileName, const ResultReader& reader, const T& expectedValue)
	{
		execute_test(testDataFileName, reader, expectedValue, default_expect_eq<T>);
	}

	template<typename ResultReader>
	void execute_test(const std::string& testDataFileName, const ResultReader& reader)
	{
		EXPECT_EQ(execute_reader(testDataFileName, reader).is_success(), true);

		EXPECT_EQ(execute_reader(ERROR_RESPONSE_FILE_NAME, reader).error(), ERROR_MESSAGE);
	}

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
		execute_test("system_status_online.json", kraken::read_system_status, exchange_status::ONLINE);
		execute_test("system_status_maintenance.json", kraken::read_system_status, exchange_status::MAINTENANCE);
		execute_test("system_status_cancel_only.json", kraken::read_system_status, exchange_status::CANCEL_ONLY);
		execute_test("system_status_post_only.json", kraken::read_system_status, exchange_status::POST_ONLY);
	}

	TEST(KrakenResults, ReadTradablePairs)
	{
		execute_test(
			"tradable_pairs_success.json",
			kraken::read_tradable_pairs,
			std::vector<tradable_pair>
		{
			tradable_pair{ "ETH", "XBT" },
			tradable_pair{ "XBT", "USD" }
		});
	}

	TEST(KrakenResults, Read24hStats)
	{
		execute_test(
			"24h_stats_success.json",
			kraken::read_24h_stats,
			pair_stats{ 369.71101684, 1917.23, 2056.45, 2034.26 },
			assert_pair_stats_eq);
	}

	TEST(KrakenResults, ReadPrice)
	{
		execute_test(
			"24h_stats_success.json",
			kraken::read_price,
			1940.0);
	}

	TEST(KrakenResults, ReadOrderBook)
	{
		execute_test(
			"order_book_success.json",
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
		execute_test(
			"balances_success.json",
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
		execute_test(
			"fee_success.json",
			kraken::read_fee,
			0.1);
	}

	TEST(KrakenResults, ReadOpenOrders)
	{
		execute_test(
			"open_orders_success.json",
			kraken::read_open_orders,
			get_expected_open_closed_orders(),
			assert_order_description_eq);
	}

	TEST(KrakenResults, ReadClosedOrders)
	{
		execute_test(
			"closed_orders_success.json",
			kraken::read_closed_orders,
			get_expected_open_closed_orders(),
			assert_order_description_eq);
	}

	TEST(KrakenResults, ReadAddOrder)
	{
		execute_test<std::string>(
			"add_order_success.json",
			kraken::read_add_order,
			"OUF4EM-FRGI2-MQMWZD");
	}

	TEST(KrakenResults, ReadCancelOrder)
	{
		execute_test("cancel_order.json", kraken::read_cancel_order);
	}
}
