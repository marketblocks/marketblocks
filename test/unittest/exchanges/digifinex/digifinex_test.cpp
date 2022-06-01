#include <gtest/gtest.h>

#include "unittest/exchanges/exchange_test_common.h"
#include "exchanges/digifinex/digifinex.h"

namespace
{
	using namespace mb;
	using namespace mb::test;

	digifinex_api create_api(http_request expectedRequest, http_response response)
	{
		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service(std::move(expectedRequest), std::move(response), IsHttpRequest<http_request>) };
		return digifinex_api{ digifinex_config{}, std::move(mockHttpService), std::make_unique<mock_websocket_stream>() };
	}
}

namespace mb::test
{
	TEST(Digifinex, GetStatus)
	{
		http_request expectedRequest
		{
			http_verb::GET,
			"https://openapi.digifinex.com/v3/ping"
		};

		http_response response
		{
			200,
			read_file(digifinex_results_test_data_path("ping.json"))
		};

		digifinex_api api{ create_api(std::move(expectedRequest), std::move(response)) };

		api.get_status();
	}

	TEST(Digifinex, GetTradablePairs)
	{
		http_request expectedRequest
		{
			http_verb::GET,
			"https://openapi.digifinex.com/v3/spot/symbols"
		};

		http_response response
		{
			200,
			read_file(digifinex_results_test_data_path("tradable_pairs.json"))
		};

		digifinex_api api{ create_api(std::move(expectedRequest), std::move(response)) };

		api.get_tradable_pairs();
	}

	TEST(Digifinex, Get24hStats)
	{
		tradable_pair pair{ "ETH", "GBP" };

		http_request expectedRequest
		{
			http_verb::GET,
			""
		};

		http_response response
		{
			200,
			read_file(digifinex_results_test_data_path("24h_stats.json"))
		};

		digifinex_api api{ create_api(std::move(expectedRequest), std::move(response)) };

		api.get_24h_stats(pair);
	}

	TEST(Digifinex, GetOrderBook)
	{
		tradable_pair pair{ "ETH", "GBP" };

		http_request expectedRequest
		{
			http_verb::GET,
			""
		};

		http_response response
		{
			200,
			read_file(digifinex_results_test_data_path("order_book.json"))
		};

		digifinex_api api{ create_api(std::move(expectedRequest), std::move(response)) };

		api.get_order_book(pair, 2);
	}

	TEST(Digifinex, GetBalances)
	{
		http_request expectedRequest
		{
			http_verb::GET,
			""
		};

		http_response response
		{
			200,
			read_file(digifinex_results_test_data_path("get_balances.json"))
		};

		digifinex_api api{ create_api(std::move(expectedRequest), std::move(response)) };

		api.get_balances();
	}

	TEST(Digifinex, GetOpenOrders)
	{
		http_request expectedRequest
		{
			http_verb::GET,
			""
		};

		http_response response
		{
			200,
			read_file(digifinex_results_test_data_path("open_orders.json"))
		};

		digifinex_api api{ create_api(std::move(expectedRequest), std::move(response)) };

		api.get_open_orders();
	}

	TEST(Digifinex, GetClosedOrders)
	{
		http_request expectedRequest
		{
			http_verb::GET,
			""
		};

		http_response response
		{
			200,
			read_file(digifinex_results_test_data_path("all_orders.json"))
		};

		digifinex_api api{ create_api(std::move(expectedRequest), std::move(response)) };

		api.get_closed_orders();
	}

	TEST(Digifinex, AddOrder)
	{
		trade_description trade{ order_type::LIMIT, tradable_pair{ "BTC", "USD" }, trade_action::BUY, 1.0, 1.0 };

		http_request expectedRequest
		{
			http_verb::POST,
			"https://openapi.digifinex.com/v3/spot/order/new?symbol=BTC_USD&type=buy&amount=1.0&price=1.0"
		};

		http_response response
		{
			200,
			read_file(digifinex_results_test_data_path("add_order.json"))
		};

		digifinex_api api{ create_api(std::move(expectedRequest), std::move(response)) };

		api.add_order(trade);
	}

	TEST(Digifinex, CancelOrder)
	{
		std::string orderId{ "198361cecdc65f9c8c9bb2fa68faec40" };
		
		http_request expectedRequest
		{
			http_verb::GET,
			""
		};

		http_response response
		{
			200,
			read_file(digifinex_results_test_data_path("cancel_order_success.json"))
		};

		digifinex_api api{ create_api(std::move(expectedRequest), std::move(response)) };

		api.cancel_order(orderId);
	}
}