#pragma once

#include <gmock/gmock.h>
#include <vector>

#include "exchanges/exchange.h"
#include "networking/http/http_service.h"

namespace mb::test
{
	class mock_http_service : public http_service
	{
	public:
		MOCK_METHOD(http_response, send, (const http_request& request), (const, override));
	};

	class mock_websocket_stream : public websocket_stream
	{
	public:
		MOCK_METHOD(void, connect, (), (override));
		MOCK_METHOD(ws_connection_status, connection_status, (), (const, override));
		MOCK_METHOD(void, subscribe_order_book, (const std::vector<tradable_pair>& tradablePairs), (override));
		MOCK_METHOD(void, unsubscribe_order_book, (const std::vector<tradable_pair>& tradablePairs), (override));
		MOCK_METHOD(bool, is_order_book_subscribed, (const tradable_pair& pair), (const, override));
		MOCK_METHOD(order_book_state, get_order_book, (const tradable_pair& pair, int depth), (const, override));
		MOCK_METHOD(set_queue<tradable_pair>&, get_order_book_message_queue, (), (noexcept, override));
	};

	class mock_exchange : public exchange
	{
	public:
		MOCK_METHOD(std::string_view, id, (), (const, override, noexcept));
		MOCK_METHOD(exchange_status, get_status, (), (const, override));
		MOCK_METHOD(std::vector<tradable_pair>, get_tradable_pairs, (), (const, override));
		MOCK_METHOD(ohlcv_data, get_24h_stats, (const tradable_pair& tradablePair), (const, override));
		MOCK_METHOD(double, get_price, (const tradable_pair& tradablePair), (const, override));
		MOCK_METHOD(order_book_state, get_order_book, (const tradable_pair& tradablePair, int depth), (const, override));
		MOCK_METHOD(unordered_string_map<double>, get_balances, (), (const, override));
		MOCK_METHOD(double, get_fee, (const tradable_pair& tradablePair), (const, override));
		MOCK_METHOD(std::vector<order_description>, get_open_orders, (), (const, override));
		MOCK_METHOD(std::vector<order_description>, get_closed_orders, (), (const, override));
		MOCK_METHOD(std::string, add_order, (const trade_description& description), (override));
		MOCK_METHOD(void, cancel_order, (std::string_view orderId), (override));
	};

	class test_strategy_base
	{
	public:
		virtual void run_iteration() {}
	};

	class mock_strategy : public test_strategy_base
	{
	public:
		MOCK_METHOD(void, run_iteration, (), (override));
	};
}
