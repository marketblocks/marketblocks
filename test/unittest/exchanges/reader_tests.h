#pragma once

#include <gtest/gtest.h>

#include "unittest/mocks.h"
#include "unittest/assertion_helpers.h"
#include "exchange_test_common.h"
#include "exchanges/exchange.h"
#include "test_data/test_data_constants.h"
#include "common/file/file.h"

namespace mb::test
{
	using ::testing::_;
	using ::testing::Return;

	template<typename Api>
	class ExchangeReaderTests : public testing::Test
	{
	protected:
		mock_http_service* _mockHttpService;
		std::unique_ptr<exchange> _api;
		tradable_pair _testingPair;

		ExchangeReaderTests()
			: _testingPair{ get_testing_pair<Api>() }
		{}

		void SetUp() override
		{
			std::unique_ptr<mock_http_service> mockHttpService{ std::make_unique<mock_http_service>() };

			_mockHttpService = mockHttpService.get();
			_api = create_exchange_api<Api>(std::move(mockHttpService), nullptr);
		}

		void set_http_response(std::string_view fileName)
		{
			std::string responseMessage{ read_response_file(_api->id(), fileName) };
			http_response response{ 200, std::move(responseMessage) };

			EXPECT_CALL(*_mockHttpService, send(_))
				.WillRepeatedly(Return(std::move(response)));
		}
	};

	TYPED_TEST_SUITE_P(ExchangeReaderTests);

	TYPED_TEST_P(ExchangeReaderTests, ReadErrorResponse)
	{
		this->set_http_response("error_response.json");
		
		try
		{
			this->_api->get_status();
		}
		catch (const mb_exception& e)
		{
			ASSERT_STREQ("This is an error", e.what());
		}
	}

	TYPED_TEST_P(ExchangeReaderTests, ReadSystemStatus)
	{
		this->set_http_response("get_status");
		exchange_status status{ this->_api->get_status() };

		ASSERT_EQ(exchange_status::ONLINE, status);
	}

	TYPED_TEST_P(ExchangeReaderTests, ReadTradablePairs)
	{
		std::vector<tradable_pair> expectedPairs
		{
			tradable_pair{ "ETH", "BTC" },
			tradable_pair{ "BTC", "USD" }
		};

		this->set_http_response("get_tradable_pairs");
		std::vector<tradable_pair> actualPairs{ this->_api->get_tradable_pairs() };

		ASSERT_EQ(expectedPairs, actualPairs);
	}

	TYPED_TEST_P(ExchangeReaderTests, ReadOhlcv)
	{
		std::vector<ohlcv_data> expectedOhlcv
		{
			ohlcv_data{ 1657043700, 19703.50, 19720.0, 19682.1, 19683.6, 3.38715290 },
			ohlcv_data{ 1657043400, 19694.5, 19757.8, 19692.1, 19707.8, 11.59346902 },
			ohlcv_data{ 1657043100, 19693.4, 19694.8, 19666.7, 19694.5, 8.42135430 }
		};

		this->set_http_response("get_ohlcv");
		std::vector<ohlcv_data> actualOhlcv{ this->_api->get_ohlcv(this->_testingPair, ohlcv_interval::M5, 3) };

		create_vector_equal_asserter<ohlcv_data>(assert_ohlcv_data_eq)(expectedOhlcv, actualOhlcv);
	}

	TYPED_TEST_P(ExchangeReaderTests, ReadPrice)
	{
		this->set_http_response("get_price");
		double price{ this->_api->get_price(this->_testingPair) };

		ASSERT_DOUBLE_EQ(20137.2, price);
	}

	TYPED_TEST_P(ExchangeReaderTests, ReadOrderBook)
	{
		order_book_state expectedOrderBook
		{
			1616663113,
			{
				order_book_entry{ 52523.0, 1.199, order_book_side::ASK },
				order_book_entry{ 52536.0, 0.30, order_book_side::ASK }
			},
			{
				order_book_entry{ 52522.9, 0.753, order_book_side::BID },
				order_book_entry{ 52522.8, 0.006, order_book_side::BID }
			}
		};

		this->set_http_response("get_order_book");
		order_book_state actualOrderBook{ this->_api->get_order_book(this->_testingPair, 2) };

		assert_order_book_state_eq(expectedOrderBook, actualOrderBook);
	}

	TYPED_TEST_P(ExchangeReaderTests, ReadFee)
	{
		this->set_http_response("get_fee");
		double fee{ this->_api->get_fee(this->_testingPair) };

		ASSERT_DOUBLE_EQ(0.20, fee);
	}

	TYPED_TEST_P(ExchangeReaderTests, ReadBalances)
	{
		std::unordered_map<std::string,double> expectedBalances
		{
			{ "USD", 171288.6158 },
			{ "BTC", 1011.1908877900 },
			{ "ETH", 818.550 }
		};

		this->set_http_response("get_balances");
		std::unordered_map<std::string,double> actualBalances{ this->_api->get_balances() };

		ASSERT_EQ(expectedBalances, actualBalances);
	}

	TYPED_TEST_P(ExchangeReaderTests, ReadOpenOrders)
	{
		std::vector<order_description> expectedOrders
		{
			order_description{ 1616665899, "1536728786697", "ETHUSD", trade_action::SELL, 1450.0, 0.275 },
			order_description{ 1616666559, "7886252369684", "BTCUSD", trade_action::BUY, 30010.0, 1.25 }
		};

		this->set_http_response("get_open_orders");
		std::vector<order_description> actualOrders{ this->_api->get_open_orders() };

		create_vector_equal_asserter<order_description>(assert_order_description_eq)(expectedOrders, actualOrders);
	}

	TYPED_TEST_P(ExchangeReaderTests, ReadClosedOrders)
	{
		std::vector<order_description> expectedOrders
		{
			order_description{ 1616666559, "7886252369684", "BTCUSD", trade_action::BUY, 30010.0, 1.25 }
		};

		this->set_http_response("get_closed_orders");
		std::vector<order_description> actualOrders{ this->_api->get_closed_orders() };

		create_vector_equal_asserter<order_description>(assert_order_description_eq)(expectedOrders, actualOrders);
	}

	TYPED_TEST_P(ExchangeReaderTests, ReadAddOrder)
	{
		this->set_http_response("add_order");
		std::string orderId{ this->_api->add_order(order_request{
			order_type::LIMIT,
			this->_testingPair,
			trade_action::BUY,
			1.0,
			1.0})};

		ASSERT_EQ("7886252369684", orderId);
	}

	TYPED_TEST_P(ExchangeReaderTests, ReadCancelOrder)
	{
		this->set_http_response("cancel_order");
		ASSERT_NO_THROW(this->_api->cancel_order("7886252369684"));
	}

	REGISTER_TYPED_TEST_SUITE_P(ExchangeReaderTests,
		ReadErrorResponse,
		ReadSystemStatus,
		ReadTradablePairs,
		ReadOhlcv,
		ReadPrice,
		ReadOrderBook,
		ReadFee,
		ReadBalances,
		ReadOpenOrders,
		ReadClosedOrders,
		ReadAddOrder,
		ReadCancelOrder);
}