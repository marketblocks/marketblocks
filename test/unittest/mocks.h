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
		mock_websocket_stream()
			: websocket_stream{ nullptr, nullptr }
		{}
	};

	class mock_exchange : public exchange
	{
	public:
		mock_exchange()
			: exchange{ mock_websocket_stream{} }
		{}

		MOCK_METHOD(std::string_view, id, (), (const, override, noexcept));
		MOCK_METHOD(exchange_status, get_status, (), (const, override));
		MOCK_METHOD(std::vector<tradable_pair>, get_tradable_pairs, (), (const, override));
		MOCK_METHOD(ohlc_data, get_24h_stats, (const tradable_pair& tradablePair), (const, override));
		MOCK_METHOD(std::vector<historical_trade>, get_historical_trades, (const tradable_pair& tradablePair, std::time_t startTime), (const, override));
		MOCK_METHOD(double, get_price, (const tradable_pair& tradablePair), (const, override));
		MOCK_METHOD(order_book_state, get_order_book, (const tradable_pair& tradablePair, int depth), (const, override));
		MOCK_METHOD(unordered_string_map<double>, get_balances, (), (const, override));
		MOCK_METHOD(double, get_fee, (const tradable_pair& tradablePair), (const, override));
		MOCK_METHOD(std::vector<order_description>, get_open_orders, (), (const, override));
		MOCK_METHOD(std::vector<order_description>, get_closed_orders, (), (const, override));
		MOCK_METHOD(std::string, add_order, (const trade_description& description), (override));
		MOCK_METHOD(void, cancel_order, (std::string_view orderId), (override));
	};
}
