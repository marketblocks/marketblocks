#include <gtest/gtest.h>

#include "exchanges/coinbase/coinbase.h"
#include "unittest/mocks.h"
#include "unittest/exchanges/exchange_test_common.h"
#include "test_data/test_data_constants.h"
#include "common/file/file.h"
#include "common/json/json.h"

namespace
{
	using namespace mb;
	using namespace mb::test;

	coinbase_api create_api(http_request expectedRequest, http_response response)
	{
		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service(std::move(expectedRequest), std::move(response), IsHttpRequest<http_request>) };
		return coinbase_api{ coinbase_config{}, std::move(mockHttpService) };
	}
}

namespace mb::test
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

		coinbase_api api{ create_api(std::move(expectedRequest), std::move(response)) };		

		api.get_tradable_pairs();
	}

	TEST(Coinbase, GetPrice)
	{
		tradable_pair pair{ "ETH", "USD" };

		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.exchange.coinbase.com/products/ETH-USD/ticker"
		};

		http_response response
		{
			200,
			read_file(coinbase_results_test_data_path("price.json"))
		};

		coinbase_api api{ create_api(std::move(expectedRequest), std::move(response)) };

		api.get_price(pair);
	}

	TEST(Coinbase, GetOrderBook)
	{
		tradable_pair pair{ "BTC", "GBP" };
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

		coinbase_api api{ create_api(std::move(expectedRequest), std::move(response)) };

		api.get_order_book(pair, depth);
	}

	TEST(Coinbase, GetFee)
	{
		tradable_pair pair{ "BTC", "GBP" };

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

		coinbase_api api{ create_api(std::move(expectedRequest), std::move(response)) };

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

		coinbase_api api{ create_api(std::move(expectedRequest), std::move(response)) };

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

		coinbase_api api{ create_api(std::move(expectedRequest), std::move(response)) };

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

		coinbase_api api{ create_api(std::move(expectedRequest), std::move(response)) };

		api.get_closed_orders();
	}

	TEST(Coinbase, AddOrder)
	{
		trade_description trade
		{
			order_type::LIMIT,
			tradable_pair{ "BTC", "USD" },
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

		coinbase_api api{ create_api(std::move(expectedRequest), std::move(response)) };

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

		coinbase_api api{ create_api(std::move(expectedRequest), std::move(response)) };

		api.cancel_order(orderId);
	}
}