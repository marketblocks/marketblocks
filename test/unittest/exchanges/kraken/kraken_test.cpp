#include <gtest/gtest.h>
#include <regex>

#include "exchanges/kraken/kraken.h"
#include "unittest/mocks.h"
#include "unittest/common_matchers.h"
#include "test_data/test_data_constants.h"
#include "common/file/file.h"

namespace
{
	using namespace mb;
	using namespace mb::test;
	using ::testing::Return;

	MATCHER_P(IsKrakenPrivateHttpRequest, expected, "")
	{
		std::string regexString{ "(nonce=)[0-9]+" };

		if (!expected.content().empty())
		{
			regexString += "&" + expected.content();
		}

		return 
			arg.verb() == expected.verb() &&
			arg.url() == expected.url() &&
			std::regex_match(arg.content(), std::regex(regexString));
	}

	template<typename HttpRequestMatcher>
	std::unique_ptr<mock_http_service> create_mock_http_service(http_request expectedRequest, http_response response, HttpRequestMatcher httpRequestMatcher)
	{
		std::unique_ptr<mock_http_service> mockHttpService{ std::make_unique<mock_http_service>() };
		EXPECT_CALL(*mockHttpService, send(httpRequestMatcher(std::move(expectedRequest))))
			.Times(1)
			.WillOnce(Return(std::move(response)));

		return mockHttpService;
	}

	std::unique_ptr<mock_http_service> create_mock_http_service_public_test(http_request expectedRequest, http_response response)
	{
		return create_mock_http_service(std::move(expectedRequest), std::move(response), IsHttpRequest<http_request>);
	}

	std::unique_ptr<mock_http_service> create_mock_http_service_private_test(http_request expectedRequest, http_response response)
	{
		return create_mock_http_service(std::move(expectedRequest), std::move(response), IsKrakenPrivateHttpRequest<http_request>);
	}
}

namespace mb::test
{
	TEST(Kraken, GetStatus)
	{
		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.kraken.com/0/public/SystemStatus"
		};

		http_response response
		{
			200,
			read_file(kraken_results_test_data_path("system_status_online.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service_public_test(std::move(expectedRequest), std::move(response)) };
		kraken_api api{ kraken_config{}, std::move(mockHttpService), mock_websocket_stream{} };

		api.get_status();
	}

	TEST(Kraken, GetTradablePairs)
	{
		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.kraken.com/0/public/AssetPairs"
		};

		http_response response
		{
			200,
			read_file(kraken_results_test_data_path("tradable_pairs_success.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service_public_test(std::move(expectedRequest), std::move(response)) };
		kraken_api api{ kraken_config{}, std::move(mockHttpService), mock_websocket_stream{} };

		api.get_tradable_pairs();
	}

	TEST(Kraken, Get24hStats)
	{
		tradable_pair pair{ "ETH", "GBP" };

		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.kraken.com/0/public/Ticker?pair=ETHGBP"
		};

		http_response response
		{
			200,
			read_file(kraken_results_test_data_path("24h_stats_success.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service_public_test(std::move(expectedRequest), std::move(response)) };
		kraken_api api{ kraken_config{}, std::move(mockHttpService), mock_websocket_stream{} };

		api.get_24h_stats(pair);
	}

	TEST(Kraken, GetHistoricalTrades)
	{
		tradable_pair pair{ "BTC", "USD" };
		time_t startTime{ 1616663618 };

		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.kraken.com/0/public/Trades?pair=BTCUSD&since=1616663618"
		};

		http_response response
		{
			200,
			read_file(kraken_results_test_data_path("historical_trades.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service_public_test(std::move(expectedRequest), std::move(response)) };
		kraken_api api{ kraken_config{}, std::move(mockHttpService), mock_websocket_stream{} };

		api.get_historical_trades(pair, startTime);
	}

	TEST(Kraken, GetHistoricalTradesWithPartialResult)
	{
		tradable_pair pair{ "BTC", "USD" };
		time_t startTime{ 1616661190 };

		http_request expectedFirstRequest
		{
			http_verb::GET,
			"https://api.kraken.com/0/public/Trades?pair=BTCUSD&since=1616661190"
		};

		http_response firstResponse
		{
			200,
			read_file(kraken_results_test_data_path("historical_trades_1000.json"))
		};

		http_request expectedSecondRequest
		{
			http_verb::GET,
			"https://api.kraken.com/0/public/Trades?pair=BTCUSD&since=1616663560178853572"
		};

		http_response secondResponse
		{
			200,
			read_file(kraken_results_test_data_path("historical_trades.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ std::make_unique<mock_http_service>() };

		EXPECT_CALL(*mockHttpService, send(IsHttpRequest(std::move(expectedFirstRequest))))
			.Times(1)
			.WillOnce(Return(std::move(firstResponse)));

		EXPECT_CALL(*mockHttpService, send(IsHttpRequest(std::move(expectedSecondRequest))))
			.Times(1)
			.WillOnce(Return(std::move(secondResponse)));

		kraken_api api{ kraken_config{}, std::move(mockHttpService), mock_websocket_stream{} };

		api.get_historical_trades(pair, startTime);
	}

	TEST(Kraken, GetOrderBook)
	{
		tradable_pair pair{ "XBT", "USD" };
		const int depth = 2;

		http_request expectedRequest
		{
			http_verb::GET,
			"https://api.kraken.com/0/public/Depth?pair=XBTUSD&count=2"
		};

		http_response response
		{
			200,
			read_file(kraken_results_test_data_path("order_book_success.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service_public_test(std::move(expectedRequest), std::move(response)) };
		kraken_api api{ kraken_config{}, std::move(mockHttpService), mock_websocket_stream{} };

		api.get_order_book(pair, depth);
	}

	TEST(Kraken, GetFee)
	{
		tradable_pair pair{ "XBT", "USD" };

		http_request expectedRequest
		{
			http_verb::POST,
			"https://api.kraken.com/0/private/TradeVolume"
		};

		expectedRequest.set_content("pair=XBTUSD");

		http_response response
		{
			200,
			read_file(kraken_results_test_data_path("fee_success.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service_private_test(std::move(expectedRequest), std::move(response)) };
		kraken_api api{ kraken_config{}, std::move(mockHttpService), mock_websocket_stream{} };

		api.get_fee(pair);
	}

	TEST(Kraken, GetBalances)
	{
		http_request expectedRequest
		{
			http_verb::POST,
			"https://api.kraken.com/0/private/Balance"
		};

		http_response response
		{
			200,
			read_file(kraken_results_test_data_path("balances_success.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service_private_test(std::move(expectedRequest), std::move(response)) };
		kraken_api api{ kraken_config{}, std::move(mockHttpService), mock_websocket_stream{} };

		api.get_balances();
	}

	TEST(Kraken, GetOpenOrders)
	{
		http_request expectedRequest
		{
			http_verb::POST,
			"https://api.kraken.com/0/private/OpenOrders"
		};

		http_response response
		{
			200,
			read_file(kraken_results_test_data_path("open_orders_success.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service_private_test(std::move(expectedRequest), std::move(response)) };
		kraken_api api{ kraken_config{}, std::move(mockHttpService), mock_websocket_stream{} };

		api.get_open_orders();
	}

	TEST(Kraken, GetClosedOrders)
	{
		http_request expectedRequest
		{
			http_verb::POST,
			"https://api.kraken.com/0/private/ClosedOrders"
		};

		http_response response
		{
			200,
			read_file(kraken_results_test_data_path("closed_orders_success.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service_private_test(std::move(expectedRequest), std::move(response)) };
		kraken_api api{ kraken_config{}, std::move(mockHttpService), mock_websocket_stream{} };

		api.get_closed_orders();
	}

	TEST(Kraken, AddOrder)
	{
		trade_description trade
		{
			order_type::LIMIT,
			tradable_pair{ "XBT", "USD" },
			trade_action::BUY,
			45000.10,
			2.12340
		};

		http_request expectedRequest
		{
			http_verb::POST,
			"https://api.kraken.com/0/private/AddOrder"
		};

		expectedRequest.set_content("pair=XBTUSD&type=buy&ordertype=limit&price=45000.100000&volume=2.123400");

		http_response response
		{
			200,
			read_file(kraken_results_test_data_path("add_order_success.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service_private_test(std::move(expectedRequest), std::move(response)) };
		kraken_api api{ kraken_config{}, std::move(mockHttpService), mock_websocket_stream{} };

		api.add_order(trade);
	}

	TEST(Kraken, CancelOrder)
	{
		std::string orderId{ "OUF4EM-FRGI2-MQMWZD" };

		http_request expectedRequest
		{
			http_verb::POST,
			"https://api.kraken.com/0/private/CancelOrder"
		};

		expectedRequest.set_content("txid=OUF4EM-FRGI2-MQMWZD");

		http_response response
		{
			200,
			read_file(kraken_results_test_data_path("cancel_order.json"))
		};

		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service_private_test(std::move(expectedRequest), std::move(response)) };
		kraken_api api{ kraken_config{}, std::move(mockHttpService), mock_websocket_stream{} };

		api.cancel_order(orderId);
	}
}