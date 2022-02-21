#pragma once

#include <gtest/gtest.h>
#include "../../assertion_helpers.h"

#include "exchanges/kraken/kraken_results.h"
#include "common/file/file.h"

namespace
{
	template<typename ResultReader>
	auto execute_reader(const std::string& dataFileName, ResultReader reader)
	{
		static const std::filesystem::path dataDirectory = "test_data\\kraken_results_test";
		std::filesystem::path dataPath = dataDirectory / dataFileName;

		std::string json = cb::read_file(dataPath);

		return reader(json);
	}

	template<typename T>
	void no_assert(const T&, const T&)
	{
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
}

namespace cb::test
{
	static const std::string ERROR_MESSAGE = "This is an error";

	TEST(KrakenResults, ReadSystemStatus)
	{
		assert_result_equal(execute_reader("read_system_status_online.json", internal::read_system_status), result<exchange_status>::success(exchange_status::ONLINE));
		assert_result_equal(execute_reader("read_system_status_maintenance.json", internal::read_system_status), result<exchange_status>::success(exchange_status::MAINTENANCE));
		assert_result_equal(execute_reader("read_system_status_cancel_only.json", internal::read_system_status), result<exchange_status>::success(exchange_status::CANCEL_ONLY));
		assert_result_equal(execute_reader("read_system_status_post_only.json", internal::read_system_status), result<exchange_status>::success(exchange_status::POST_ONLY));
		assert_result_equal(execute_reader("error_response.json", internal::read_system_status), result<exchange_status>::fail(ERROR_MESSAGE));
	}

	TEST(KrakenResults, ReadTradablePairs)
	{
		assert_result_equal(
			execute_reader("read_tradable_pairs_success.json", internal::read_tradable_pairs),
			result<std::vector<tradable_pair>>::success(
				{ 
					tradable_pair{ asset_symbol{"ETH"}, asset_symbol{"XBT"} },
					tradable_pair{asset_symbol{"XBT"}, asset_symbol{"USD"}} 
				}));

		assert_result_equal(
			execute_reader("error_response.json", internal::read_tradable_pairs),
			result<std::vector<tradable_pair>>::fail(ERROR_MESSAGE));
	}

	TEST(KrakenResults, ReadTickerData)
	{
		assert_result_equal(
			execute_reader("read_ticker_data_success.json", internal::read_ticker_data),
			result<ticker_data>::success(ticker_data{ 1940.0, 3.0, 1939.990, 9.0, 267.88525096, 369.71101684, 560, 809, 1917.23, 1917.23, 2034.26, 2056.45, 2034.26 }),
			assert_ticker_data_eq);

		assert_result_equal(
			execute_reader("error_response.json", internal::read_ticker_data),
			result<ticker_data>::fail(ERROR_MESSAGE),
			no_assert<ticker_data>);
	}

	TEST(KrakenResults, ReadOrderBook)
	{
		assert_result_equal(
			execute_reader("read_order_book_success.json", internal::read_order_book),
			result<order_book_state>::success(order_book_state{
				{
					order_book_level{ order_book_entry{order_book_side::ASK, 52523.0, 1.199, 1616663113}, order_book_entry{order_book_side::BID, 52522.9, 0.753, 1616663112} },
					order_book_level{ order_book_entry{order_book_side::ASK, 52536.0, 0.30, 1616663112}, order_book_entry{order_book_side::BID, 52522.8, 0.006, 1616663109} }
				} }),
			assert_order_book_state_eq);

		assert_result_equal(
			execute_reader("error_response.json", internal::read_order_book),
			result<order_book_state>::fail(ERROR_MESSAGE),
			no_assert<order_book_state>);
	}

	TEST(KrakenResults, ReadBalances)
	{
		assert_result_equal(
			execute_reader("read_balances_success.json", internal::read_balances),
			result<std::unordered_map<asset_symbol, double>>::success(
				{
					{ asset_symbol{ "ZUSD" }, 171288.6158 },
					{ asset_symbol{ "ZEUR" }, 504861.8946 },
					{ asset_symbol{ "ZGBP" }, 459567.9171 },
					{ asset_symbol{ "XXBT" }, 1011.19088779 },
					{ asset_symbol{ "XLTC" }, 2000.0 },
					{ asset_symbol{ "XETH" }, 818.55 }
				}));
		
		assert_result_equal(
			execute_reader("error_response.json", internal::read_balances),
			result<std::unordered_map<asset_symbol, double>>::fail(ERROR_MESSAGE));
	}

	TEST(KrakenResults, ReadFee)
	{
		assert_result_equal(
			execute_reader("read_fee_success.json", internal::read_fee),
			result<double>::success(0.1));

		assert_result_equal(
			execute_reader("error_response.json", internal::read_fee),
			result<double>::fail(ERROR_MESSAGE));
	}
}
