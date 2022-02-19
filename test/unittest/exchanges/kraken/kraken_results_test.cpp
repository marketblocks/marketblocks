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
			assert_order_book_state_eq);
	}
}
