#include <gtest/gtest.h>

#include "exchanges/coinbase/coinbase.h"
#include "unittest/mocks.h"
#include "unittest/common_matchers.h"
#include "test_data/test_data_constants.h"
#include "common/file/file.h"
#include "common/file/json.h"

namespace
{
	using namespace cb::test;
	using ::testing::Return;

	std::unique_ptr<mock_http_service> create_mock_http_service(cb::http_request expectedRequest, cb::http_response response)
	{
		std::unique_ptr<mock_http_service> mockHttpService{ std::make_unique<mock_http_service>() };
		EXPECT_CALL(*mockHttpService, send(IsHttpRequest(std::move(expectedRequest))))
			.Times(1)
			.WillOnce(Return(std::move(response)));

		return mockHttpService;
	}
}

namespace cb::test
{
	TEST(Coinbase, GetTradablePairs)
	{
		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.exchange.coinbase.com/products"
		};

		http_response response
		{
			200,
			read_file(coinbase_results_test_data_path("tradable_pairs.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service(std::move(expectedRequest), std::move(response)) };
		coinbase_api api{ coinbase_config{}, std::move(mockHttpService), std::make_unique<mock_websocket_stream>() };

		api.get_tradable_pairs();
	}

	TEST(Coinbase, Get24hStats)
	{
		tradable_pair pair{ "ETH-USD", asset_symbol{ "ETH" }, asset_symbol{ "USD" } };

		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.exchange.coinbase.com/products/ETH-USD/stats"
		};

		http_response response
		{
			200,
			read_file(coinbase_results_test_data_path("24h_stats.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service(std::move(expectedRequest), std::move(response)) };
		coinbase_api api{ coinbase_config{}, std::move(mockHttpService), std::make_unique<mock_websocket_stream>() };

		api.get_24h_stats(pair);
	}

	TEST(Coinbase, GetOrderBook)
	{
		tradable_pair pair{ "BTC-GBP", asset_symbol{ "BTC" }, asset_symbol{ "GBP" } };
		const int depth = 3;

		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.exchange.coinbase.com/products/BTC-GBP/book?level=2"
		};

		http_response response
		{
			200,
			read_file(coinbase_results_test_data_path("order_book.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service(std::move(expectedRequest), std::move(response)) };
		coinbase_api api{ coinbase_config{}, std::move(mockHttpService), std::make_unique<mock_websocket_stream>() };

		api.get_order_book(pair, depth);
	}

	TEST(Coinbase, GetFee)
	{
		tradable_pair pair{ "BTC-GBP", asset_symbol{ "BTC" }, asset_symbol{ "GBP" } };

		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.exchange.coinbase.com/fees"
		};

		http_response response
		{
			200,
			read_file(coinbase_results_test_data_path("get_fee.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service(std::move(expectedRequest), std::move(response)) };
		coinbase_api api{ coinbase_config{}, std::move(mockHttpService), std::make_unique<mock_websocket_stream>() };

		api.get_fee(pair);
	}

	TEST(Coinbase, GetBalances)
	{
		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.exchange.coinbase.com/coinbase-accounts"
		};

		http_response response
		{
			200,
			read_file(coinbase_results_test_data_path("get_balances.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service(std::move(expectedRequest), std::move(response)) };
		coinbase_api api{ coinbase_config{}, std::move(mockHttpService), std::make_unique<mock_websocket_stream>() };

		api.get_balances();
	}

	TEST(Coinbase, GetOpenOrders)
	{
		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.exchange.coinbase.com/orders"
		};

		http_response response
		{
			200,
			read_file(coinbase_results_test_data_path("get_orders.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service(std::move(expectedRequest), std::move(response)) };
		coinbase_api api{ coinbase_config{}, std::move(mockHttpService), std::make_unique<mock_websocket_stream>() };

		api.get_open_orders();
	}

	TEST(Coinbase, GetClosedOrders)
	{
		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.exchange.coinbase.com/orders?status=done"
		};

		http_response response
		{
			200,
			read_file(coinbase_results_test_data_path("get_orders.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service(std::move(expectedRequest), std::move(response)) };
		coinbase_api api{ coinbase_config{}, std::move(mockHttpService), std::make_unique<mock_websocket_stream>() };

		api.get_closed_orders();
	}

	TEST(Coinbase, AddOrder)
	{
		trade_description trade
		{
			order_type::LIMIT,
			tradable_pair{ "BTC-USD", asset_symbol{ "BTC" }, asset_symbol{ "USD" } },
			trade_action::BUY,
			10.0,
			1.0
		};

		http_request expectedRequest
		{
			http_verb::POST,
			"https://api.exchange.coinbase.com/orders"
		};

		json_writer jsonWriter;
		jsonWriter.add("type", "limit");
		jsonWriter.add("side", "buy");
		jsonWriter.add("product_id", "BTC-USD");
		jsonWriter.add("price", "10.000000");
		jsonWriter.add("size", "1.000000");

		expectedRequest.set_content(jsonWriter.to_string());

		http_response response
		{
			200,
			read_file(coinbase_results_test_data_path("add_order.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service(std::move(expectedRequest), std::move(response)) };
		coinbase_api api{ coinbase_config{}, std::move(mockHttpService), std::make_unique<mock_websocket_stream>() };

		api.add_order(trade);
	}

	TEST(Coinbase, CancelOrder)
	{
		std::string orderId = "a9625b04-fc66-4999-a876-543c3684d702";

		http_request expectedRequest
		{
			http_verb::HTTP_DELETE,
			"https://api.exchange.coinbase.com/orders/a9625b04-fc66-4999-a876-543c3684d702"
		};

		http_response response
		{
			200,
			read_file(coinbase_results_test_data_path("cancel_order.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service(std::move(expectedRequest), std::move(response)) };
		coinbase_api api{ coinbase_config{}, std::move(mockHttpService), std::make_unique<mock_websocket_stream>() };

		api.cancel_order(orderId);
	}
}