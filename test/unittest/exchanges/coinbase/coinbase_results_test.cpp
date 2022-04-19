#include <gtest/gtest.h>

#include "unittest/assertion_helpers.h"
#include "test_data/test_data_constants.h"
#include "exchanges/coinbase/coinbase_results.h"

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
		std::filesystem::path dataPath{ coinbase_results_test_data_path(dataFileName) };
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
}

namespace mb::test
{
	TEST(CoinbaseResults, ReadTradablePairs)
	{
		execute_test(
			"tradable_pairs.json",
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
			"24h_stats.json",
			coinbase::read_24h_stats,
			ohlc_data{ 5414.18, 6441.37, 5261.69, 6250.02, 53687.76764233 },
			assert_pair_stats_eq);
	}

	TEST(CoinbaseResults, ReadHistoricalTrades)
	{
		execute_test(
			"historical_trades.json",
			coinbase::read_historical_trades,
			coinbase::historical_trades_result
			{
				std::vector<historical_trade>
				{
					historical_trade{ std::time_t{ 1650302643 }, trade_action::SELL, 30694.92, 0.000057 },
					historical_trade{ std::time_t{ 1650302633 }, trade_action::SELL, 30704.07, 0.01125099 },
					historical_trade{ std::time_t{ 1650302633 }, trade_action::BUY, 30699.85, 0.11486104 },
					historical_trade{ std::time_t{ 1650302573 }, trade_action::SELL, 30699.83, 0.02763573 },
				},
				29740531
			},
			create_partial_data_result_asserter<historical_trade, int>(assert_historical_trade_eq));
	}

	TEST(CoinbaseResults, ReadPrice)
	{
		execute_test(
			"price.json",
			coinbase::read_price,
			6268.48);
	}

	TEST(CoinbaseResults, ReadOrderBook)
	{
		constexpr int depth = 2;

		execute_test(
			"order_book.json",
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
			"get_fee.json",
			coinbase::read_fee,
			0.60);
	}

	TEST(CoinbaseResults, ReadBalances)
	{
		execute_test(
			"get_balances.json",
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
			"get_orders.json",
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
			"add_order.json",
			coinbase::read_add_order,
			"a9625b04-fc66-4999-a876-543c3684d702");
	}

	TEST(CoinbaseResults, ReadCancelOrder)
	{
		EXPECT_EQ(execute_reader("cancel_order.json", coinbase::read_cancel_order).is_success(), true);

		EXPECT_EQ(execute_reader(ERROR_RESPONSE_FILE_NAME, coinbase::read_cancel_order).error(), ERROR_MESSAGE);
	}
}