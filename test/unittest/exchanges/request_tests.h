#pragma once

#include <gtest/gtest.h>

#include "unittest/mocks.h"
#include "unittest/assertion_helpers.h"
#include "exchange_test_common.h"
#include "exchanges/exchange.h"
#include "test_data/test_data_constants.h"
#include "common/file/file.h"

namespace
{
	using namespace mb;

	http_request parse_http_request(std::string requestString)
	{
		json_document json{ parse_json(requestString) };

		http_request request
		{
			to_http_verb(json.get<std::string>("verb")),
			json.get<std::string>("url")
		};

		request.set_content(json.get<std::string>("content"));

		return request;
	}

	bool request_string_eq(std::string expected, std::string actual)
	{
		constexpr const char escape = '<';
		constexpr const char close = '>';

		size_t start = 0;
		while (expected.contains(escape))
		{
			start = expected.find_first_of(escape, start);
			size_t end = expected.find_first_of(close, start);
			
			int count = std::stoi(expected.substr(start + 1, end - start - 1));

			expected.erase(start, end - start + 1);
			actual.erase(start, count);
		}

		return expected == actual;
	}

	MATCHER_P(IsHttpRequest, expected, "")
	{
		return
			arg.verb() == expected.verb() &&
			request_string_eq(expected.url(), arg.url()) &&
			request_string_eq(expected.content(), arg.content());
	}
}

namespace mb::test
{
	using ::testing::_;

	template<typename Api>
	class ExchangeRequestTests : public testing::Test
	{
	protected:
		mock_http_service* _mockHttpService;
		std::unique_ptr<exchange> _api;
		tradable_pair _testingPair;

		ExchangeRequestTests()
			: _testingPair{ get_testing_pair<Api>() }
		{}

		void SetUp() override
		{
			std::unique_ptr<mock_http_service> mockHttpService{ std::make_unique<mock_http_service>() };

			_mockHttpService = mockHttpService.get();
			_api = create_exchange_api<Api>(std::move(mockHttpService), nullptr);
		}

		void set_http_service_behaviour(std::string_view fileName)
		{
			std::string requestFile{ read_request_file(_api->id(), fileName) };

			if (requestFile.empty())
			{
				return;
			}

			http_request expectedRequest{ parse_http_request(std::move(requestFile)) };

			std::string responseFile{ read_response_file(_api->id(), fileName) };
			http_response response{ 200, std::move(responseFile) };

			EXPECT_CALL(*_mockHttpService, send(IsHttpRequest(std::move(expectedRequest))))
				.WillRepeatedly(Return(std::move(response)));
		}
	};

	TYPED_TEST_SUITE_P(ExchangeRequestTests);

	TYPED_TEST_P(ExchangeRequestTests, GetStatus)
	{
		this->set_http_service_behaviour("get_status");
		this->_api->get_status();
	}

	TYPED_TEST_P(ExchangeRequestTests, GetTradablePairs)
	{
		this->set_http_service_behaviour("get_tradable_pairs");
		this->_api->get_tradable_pairs();
	}

	TYPED_TEST_P(ExchangeRequestTests, GetOhlcv)
	{
		this->set_http_service_behaviour("get_ohlcv");
		this->_api->get_ohlcv(tradable_pair{ "BTC", "USD" }, ohlcv_interval::M5, 10);
	}

	TYPED_TEST_P(ExchangeRequestTests, GetPrice)
	{
		this->set_http_service_behaviour("get_price");
		this->_api->get_price(tradable_pair{ "BTC", "USD" });
	}

	TYPED_TEST_P(ExchangeRequestTests, GetOrderBook)
	{
		this->set_http_service_behaviour("get_order_book");
		this->_api->get_order_book(tradable_pair{ "BTC", "USD" }, 5);
	}

	TYPED_TEST_P(ExchangeRequestTests, GetFee)
	{
		this->set_http_service_behaviour("get_fee");
		this->_api->get_fee(tradable_pair{ "BTC", "USD" });
	}

	TYPED_TEST_P(ExchangeRequestTests, GetBalances)
	{
		this->set_http_service_behaviour("get_balances");
		this->_api->get_balances();
	}

	TYPED_TEST_P(ExchangeRequestTests, GetOpenOrders)
	{
		this->set_http_service_behaviour("get_open_orders");
		this->_api->get_open_orders();
	}

	TYPED_TEST_P(ExchangeRequestTests, GetClosedOrders)
	{
		this->set_http_service_behaviour("get_closed_orders");
		this->_api->get_closed_orders();
	}

	TYPED_TEST_P(ExchangeRequestTests, AddOrder)
	{
		this->set_http_service_behaviour("add_order");
		this->_api->add_order(
			trade_description
			{
				order_type::LIMIT,
				tradable_pair{ "BTC", "USD" },
				trade_action::BUY,
				1234.56,
				0.789
			});
	}

	TYPED_TEST_P(ExchangeRequestTests, CancelOrder)
	{
		this->set_http_service_behaviour("cancel_order");
		this->_api->cancel_order("123456789");
	}

	REGISTER_TYPED_TEST_SUITE_P(ExchangeRequestTests,
		GetStatus,
		GetTradablePairs,
		GetOhlcv,
		GetPrice,
		GetOrderBook,
		GetFee,
		GetBalances,
		GetOpenOrders,
		GetClosedOrders,
		AddOrder,
		CancelOrder);
}