#pragma once

#include <gtest/gtest.h>
#include "unittest/assertion_helpers.h"
#include "test_data/test_data_constants.h"

#include "exchanges/kraken/kraken_results.h"
#include "common/file/file.h"

namespace
{
	using namespace cb::test;
	
	static const std::string ERROR_MESSAGE = "This is an error";
	static const std::string ERROR_RESPONSE_FILE_NAME = "error_response.json";

	template<typename ResultReader>
	auto execute_reader(const std::string& dataFileName, ResultReader reader)
	{
		std::filesystem::path dataPath{ kraken_results_test_data_path(dataFileName) };
		std::string json = cb::read_file(dataPath);

		return reader(json);
	}

	template<typename T, typename ResultReader, typename ValueAsserter>
	void execute_test(const std::string& testDataFileName, const ResultReader& reader, T expectedValue, const ValueAsserter& valueAsserter)
	{
		assert_result_equal(
			execute_reader(testDataFileName, reader),
			cb::result<T>::success(std::move(expectedValue)),
			valueAsserter);

		assert_result_equal(
			execute_reader(ERROR_RESPONSE_FILE_NAME, reader),
			cb::result<T>::fail(ERROR_MESSAGE),
			no_assert<T>);
	}

	template<typename T, typename ResultReader>
	void execute_test(const std::string& testDataFileName, const ResultReader& reader, const T& expectedValue)
	{
		execute_test(testDataFileName, reader, expectedValue, cb::test::default_expect_eq<T>);
	}

	template<typename ResultReader>
	void execute_test(const std::string& testDataFileName, const ResultReader& reader)
	{
		EXPECT_EQ(execute_reader(testDataFileName, reader).is_success(), true);

		EXPECT_EQ(execute_reader(ERROR_RESPONSE_FILE_NAME, reader).error(), ERROR_MESSAGE);
	}

	void assert_order_book_entry_eq(const cb::order_book_entry& lhs, const cb::order_book_entry& rhs)
	{
		EXPECT_EQ(lhs.side(), rhs.side());
		EXPECT_DOUBLE_EQ(lhs.price(), rhs.price());
		EXPECT_DOUBLE_EQ(lhs.volume(), rhs.volume());
		EXPECT_DOUBLE_EQ(lhs.time_stamp(), rhs.time_stamp());
	}

	void assert_order_book_level_eq(const cb::order_book_level& lhs, const cb::order_book_level& rhs)
	{
		assert_order_book_entry_eq(lhs.ask(), rhs.ask());
		assert_order_book_entry_eq(lhs.bid(), rhs.bid());
	}

	void assert_order_book_state_eq(const cb::order_book_state& lhs, const cb::order_book_state& rhs)
	{
		ASSERT_EQ(lhs.depth(), rhs.depth());

		for (int i = 0; i < lhs.depth(); ++i)
		{
			assert_order_book_level_eq(lhs.level(i), rhs.level(i));
		}
	}

	void assert_ticker_data_eq(const cb::ticker_data& lhs, const cb::ticker_data& rhs)
	{
		EXPECT_DOUBLE_EQ(lhs.ask_price(), rhs.ask_price());
		EXPECT_DOUBLE_EQ(lhs.ask_volume(), rhs.ask_volume());
		EXPECT_DOUBLE_EQ(lhs.bid_price(), rhs.bid_price());
		EXPECT_DOUBLE_EQ(lhs.bid_volume(), rhs.bid_volume());
		EXPECT_DOUBLE_EQ(lhs.high_24(), rhs.high_24());
		EXPECT_DOUBLE_EQ(lhs.high_today(), rhs.high_today());
		EXPECT_DOUBLE_EQ(lhs.low_24(), rhs.low_24());
		EXPECT_DOUBLE_EQ(lhs.low_today(), rhs.low_today());
		EXPECT_DOUBLE_EQ(lhs.opening_price(), rhs.opening_price());
		EXPECT_EQ(lhs.trades_24(), rhs.trades_24());
		EXPECT_EQ(lhs.trades_today(), rhs.trades_today());
		EXPECT_DOUBLE_EQ(lhs.volume_24(), rhs.volume_24());
		EXPECT_DOUBLE_EQ(lhs.volume_today(), rhs.volume_today());
	}

	std::vector<cb::order_description> get_expected_open_closed_orders()
	{
		return
		{
			cb::order_description{ "OB5VMB-B4U2U-DK2WRW", "XBTUSD", cb::trade_action::SELL, 14500.0, 0.275 },
			cb::order_description{ "OQCLML-BW3P3-BUCMWZ", "XBTUSD", cb::trade_action::BUY, 30010.0, 1.25 }
		};
	}

	void assert_order_description_eq(const std::vector<cb::order_description>& lhs, const std::vector<cb::order_description>& rhs)
	{
		ASSERT_EQ(lhs.size(), rhs.size());

		for (int i = 0; i < lhs.size(); ++i)
		{
			EXPECT_EQ(lhs[i].order_id(), rhs[i].order_id());
			EXPECT_EQ(lhs[i].pair_name(), rhs[i].pair_name());
			EXPECT_EQ(lhs[i].action(), rhs[i].action());
			EXPECT_DOUBLE_EQ(lhs[i].price(), rhs[i].price());
			EXPECT_DOUBLE_EQ(lhs[i].volume(), rhs[i].volume());
		}
	}
}

namespace cb::test
{
	TEST(KrakenResults, ReadSystemStatus)
	{
		execute_test("system_status_online.json", internal::read_system_status, exchange_status::ONLINE);
		execute_test("system_status_maintenance.json", internal::read_system_status, exchange_status::MAINTENANCE);
		execute_test("system_status_cancel_only.json", internal::read_system_status, exchange_status::CANCEL_ONLY);
		execute_test("system_status_post_only.json", internal::read_system_status, exchange_status::POST_ONLY);
	}

	TEST(KrakenResults, ReadTradablePairs)
	{
		execute_test(
			"tradable_pairs_success.json",
			internal::read_tradable_pairs,
			std::vector<tradable_pair>
		{
			tradable_pair{ asset_symbol{"ETH"}, asset_symbol{"XBT"} },
				tradable_pair{ asset_symbol{"XBT"}, asset_symbol{"USD"} }
		});
	}

	TEST(KrakenResults, ReadTickerData)
	{
		execute_test(
			"ticker_data_success.json",
			internal::read_ticker_data,
			ticker_data{ 1940.0, 3.0, 1939.990, 9.0, 267.88525096, 369.71101684, 560, 809, 1917.23, 1917.23, 2034.26, 2056.45, 2034.26 },
			assert_ticker_data_eq);
	}

	TEST(KrakenResults, ReadOrderBook)
	{
		execute_test(
			"order_book_success.json",
			internal::read_order_book,
			order_book_state
			{
				{
					order_book_level{ order_book_entry{order_book_side::ASK, 52523.0, 1.199, 1616663113}, order_book_entry{order_book_side::BID, 52522.9, 0.753, 1616663112} },
					order_book_level{ order_book_entry{order_book_side::ASK, 52536.0, 0.30, 1616663112}, order_book_entry{order_book_side::BID, 52522.8, 0.006, 1616663109} }
				}
			},
			assert_order_book_state_eq);
	}

	TEST(KrakenResults, ReadBalances)
	{
		execute_test(
			"balances_success.json",
			internal::read_balances,
			std::unordered_map<asset_symbol, double>
		{
			{ asset_symbol{ "ZUSD" }, 171288.6158 },
			{ asset_symbol{ "ZEUR" }, 504861.8946 },
			{ asset_symbol{ "ZGBP" }, 459567.9171 },
			{ asset_symbol{ "XXBT" }, 1011.19088779 },
			{ asset_symbol{ "XLTC" }, 2000.0 },
			{ asset_symbol{ "XETH" }, 818.55 }
		});
	}

	TEST(KrakenResults, ReadFee)
	{
		execute_test(
			"fee_success.json",
			internal::read_fee,
			0.1);
	}

	TEST(KrakenResults, ReadOpenOrders)
	{
		execute_test(
			"open_orders_success.json",
			internal::read_open_orders,
			get_expected_open_closed_orders(),
			assert_order_description_eq);
	}

	TEST(KrakenResults, ReadClosedOrders)
	{
		execute_test(
			"closed_orders_success.json",
			internal::read_closed_orders,
			get_expected_open_closed_orders(),
			assert_order_description_eq);
	}

	TEST(KrakenResults, ReadAddOrder)
	{
		execute_test<std::string>(
			"add_order_success.json",
			internal::read_add_order,
			"OUF4EM-FRGI2-MQMWZD");
	}

	TEST(KrakenResults, ReadCancelOrder)
	{
		execute_test("cancel_order.json", internal::read_cancel_order);
	}
}
