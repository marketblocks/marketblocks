//#include <gtest/gtest.h>
//#include <regex>
//
//#include "exchanges/kraken/kraken.h"
//#include "unittest/mocks.h"
//#include "unittest/common_matchers.h"
//#include "unittest/exchanges/exchange_test_common.h"
//#include "test_data/test_data_constants.h"
//#include "common/file/file.h"
//
//namespace
//{
//	using namespace mb;
//	using namespace mb::test;
//	using ::testing::Return;
//
//	MATCHER_P(IsKrakenPrivateHttpRequest, expected, "")
//	{
//		std::string regexString{ "(nonce=)[0-9]+" };
//
//		if (!expected.content().empty())
//		{
//			regexString += "&" + expected.content();
//		}
//
//		return 
//			arg.verb() == expected.verb() &&
//			arg.url() == expected.url() &&
//			std::regex_match(arg.content(), std::regex(regexString));
//	}
//
//	template<typename HttpRequestMatcher>
//	kraken_api create_api(http_request expectedRequest, http_response response, HttpRequestMatcher matcher)
//	{
//		std::unique_ptr<mock_http_service> mockHttpService{ create_mock_http_service(std::move(expectedRequest), std::move(response), matcher) };
//		return kraken_api{ kraken_config{}, std::move(mockHttpService), nullptr, true };
//	}
//}
//
//namespace mb::test
//{
//	TEST(Kraken, GetStatus)
//	{
//		http_request expectedRequest
//		{
//			http_verb::GET,
//			"https://api.kraken.com/0/public/SystemStatus"
//		};
//
//		http_response response
//		{
//			200,
//			read_file(kraken_results_test_data_path("system_status_online.json"))
//		};
//
//		kraken_api api{ create_api(std::move(expectedRequest), std::move(response), IsHttpRequest<http_request>) };
//
//		api.get_status();
//	}
//
//	TEST(Kraken, GetTradablePairs)
//	{
//		http_request expectedRequest
//		{
//			http_verb::GET,
//			"https://api.kraken.com/0/public/AssetPairs"
//		};
//
//		http_response response
//		{
//			200,
//			read_file(kraken_results_test_data_path("tradable_pairs_success.json"))
//		};
//
//		kraken_api api{ create_api(std::move(expectedRequest), std::move(response), IsHttpRequest<http_request>) };
//
//		api.get_tradable_pairs();
//	}
//
//	TEST(Kraken, GetOrderBook)
//	{
//		tradable_pair pair{ "XBT", "USD" };
//		const int depth = 2;
//
//		http_request expectedRequest
//		{
//			http_verb::GET,
//			"https://api.kraken.com/0/public/Depth?pair=XBTUSD&count=2"
//		};
//
//		http_response response
//		{
//			200,
//			read_file(kraken_results_test_data_path("order_book_success.json"))
//		};
//
//		kraken_api api{ create_api(std::move(expectedRequest), std::move(response), IsHttpRequest<http_request>) };
//
//		api.get_order_book(pair, depth);
//	}
//
//	TEST(Kraken, GetFee)
//	{
//		tradable_pair pair{ "XBT", "USD" };
//
//		http_request expectedRequest
//		{
//			http_verb::POST,
//			"https://api.kraken.com/0/private/TradeVolume"
//		};
//
//		expectedRequest.set_content("pair=XBTUSD");
//
//		http_response response
//		{
//			200,
//			read_file(kraken_results_test_data_path("fee_success.json"))
//		};
//
//		kraken_api api{ create_api(std::move(expectedRequest), std::move(response), IsKrakenPrivateHttpRequest<http_request>) };
//
//		api.get_fee(pair);
//	}
//
//	TEST(Kraken, GetBalances)
//	{
//		http_request expectedRequest
//		{
//			http_verb::POST,
//			"https://api.kraken.com/0/private/Balance"
//		};
//
//		http_response response
//		{
//			200,
//			read_file(kraken_results_test_data_path("balances_success.json"))
//		};
//
//		kraken_api api{ create_api(std::move(expectedRequest), std::move(response), IsKrakenPrivateHttpRequest<http_request>) };
//
//		api.get_balances();
//	}
//
//	TEST(Kraken, GetOpenOrders)
//	{
//		http_request expectedRequest
//		{
//			http_verb::POST,
//			"https://api.kraken.com/0/private/OpenOrders"
//		};
//
//		http_response response
//		{
//			200,
//			read_file(kraken_results_test_data_path("open_orders_success.json"))
//		};
//
//		kraken_api api{ create_api(std::move(expectedRequest), std::move(response), IsKrakenPrivateHttpRequest<http_request>) };
//
//		api.get_open_orders();
//	}
//
//	TEST(Kraken, GetClosedOrders)
//	{
//		http_request expectedRequest
//		{
//			http_verb::POST,
//			"https://api.kraken.com/0/private/ClosedOrders"
//		};
//
//		http_response response
//		{
//			200,
//			read_file(kraken_results_test_data_path("closed_orders_success.json"))
//		};
//
//		kraken_api api{ create_api(std::move(expectedRequest), std::move(response), IsKrakenPrivateHttpRequest<http_request>) };
//
//		api.get_closed_orders();
//	}
//
//	TEST(Kraken, AddOrder)
//	{
//		trade_description trade
//		{
//			order_type::LIMIT,
//			tradable_pair{ "XBT", "USD" },
//			trade_action::BUY,
//			45000.10,
//			2.12340
//		};
//
//		http_request expectedRequest
//		{
//			http_verb::POST,
//			"https://api.kraken.com/0/private/AddOrder"
//		};
//
//		expectedRequest.set_content("pair=XBTUSD&type=buy&ordertype=limit&price=45000.100000&volume=2.123400");
//
//		http_response response
//		{
//			200,
//			read_file(kraken_results_test_data_path("add_order_success.json"))
//		};
//
//		kraken_api api{ create_api(std::move(expectedRequest), std::move(response), IsKrakenPrivateHttpRequest<http_request>) };
//
//		api.add_order(trade);
//	}
//
//	TEST(Kraken, CancelOrder)
//	{
//		std::string orderId{ "OUF4EM-FRGI2-MQMWZD" };
//
//		http_request expectedRequest
//		{
//			http_verb::POST,
//			"https://api.kraken.com/0/private/CancelOrder"
//		};
//
//		expectedRequest.set_content("txid=OUF4EM-FRGI2-MQMWZD");
//
//		http_response response
//		{
//			200,
//			read_file(kraken_results_test_data_path("cancel_order.json"))
//		};
//
//		kraken_api api{ create_api(std::move(expectedRequest), std::move(response), IsKrakenPrivateHttpRequest<http_request>) };
//
//		api.cancel_order(orderId);
//	}
//}