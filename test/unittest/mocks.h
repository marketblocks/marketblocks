#pragma once

#include <gmock/gmock.h>
#include <vector>

#include "exchanges/exchange.h"
#include "exchanges/websockets/exchange_websocket_stream.h"
#include "networking/http/http_service.h"
#include "networking/websocket/websocket_connection.h"
#include "testing/back_testing/data_loading/back_testing_data_source.h"

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
		MOCK_METHOD(void, reset, (), (override));
		MOCK_METHOD(void, disconnect, (), (override));
		MOCK_METHOD(ws_connection_status, connection_status, (), (const, override));

		MOCK_METHOD(void, subscribe, (const websocket_subscription& subscription), (override));
		MOCK_METHOD(void, unsubscribe, (const websocket_subscription& subscription), (override));
		MOCK_METHOD(subscription_status, get_subscription_status, (const unique_websocket_subscription& subscription), (const, override));

		MOCK_METHOD(order_book_state, get_order_book, (const tradable_pair& pair, int depth), (const, override));
		MOCK_METHOD(trade_update, get_last_trade, (const tradable_pair& pair), (const, override));
		MOCK_METHOD(ohlcv_data, get_last_candle, (const tradable_pair& pair, ohlcv_interval interval), (const, override));
	};

	class mock_exchange_websocket_stream : public exchange_websocket_stream
	{
	public:
		mock_exchange_websocket_stream(
			std::string_view id,
			std::string url,
			std::unique_ptr<websocket_connection_factory> connectionFactory)
			: exchange_websocket_stream{ id, std::move(url), '\0', std::move(connectionFactory)}
		{}

		MOCK_METHOD(void, on_message, (std::string_view message), (override));
		MOCK_METHOD(void, send_subscribe, (const websocket_subscription& subscription), (override));
		MOCK_METHOD(void, send_unsubscribe, (const websocket_subscription& subscription), (override));

		void expose_update_trade(std::string pairName, trade_update trade)
		{
			update_trade(std::move(pairName), std::move(trade));
		}

		void expose_update_ohlcv(std::string pairName, ohlcv_interval interval, ohlcv_data data)
		{
			update_ohlcv(std::move(pairName), interval, std::move(data));
		}

		void expose_initialise_order_book(std::string pairName, order_book_cache cache)
		{
			initialise_order_book(std::move(pairName), std::move(cache));
		}

		void expose_update_order_book(std::string pairName, std::time_t timeStamp, order_book_entry entry)
		{
			update_order_book(std::move(pairName), timeStamp, std::move(entry));
		}

		void expose_set_unsubscribed(const named_subscription& subscription)
		{
			set_unsubscribed(subscription);
		}
	};

	class mock_exchange : public exchange
	{
	public:
		mock_exchange()
			: exchange{ "TEST", nullptr }
		{}

		MOCK_METHOD(exchange_status, get_status, (), (const, override));
		MOCK_METHOD(std::vector<tradable_pair>, get_tradable_pairs, (), (const, override));
		MOCK_METHOD(std::vector<ohlcv_data>, get_ohlcv, (const tradable_pair& tradablePair, ohlcv_interval interval, int count), (const, override));
		MOCK_METHOD(double, get_price, (const tradable_pair& tradablePair), (const, override));
		MOCK_METHOD(order_book_state, get_order_book, (const tradable_pair& tradablePair, int depth), (const, override));
		MOCK_METHOD((std::unordered_map<std::string,double>), get_balances, (), (const, override));
		MOCK_METHOD(double, get_fee, (const tradable_pair& tradablePair), (const, override));
		MOCK_METHOD(std::vector<order_description>, get_open_orders, (), (const, override));
		MOCK_METHOD(std::vector<order_description>, get_closed_orders, (), (const, override));
		MOCK_METHOD(std::string, add_order, (const order_request& description), (override));
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

	class mock_websocket_connection_factory : public websocket_connection_factory
	{
	public:
		void fire_on_message(std::string_view message) { _onMessage(message); }

		MOCK_METHOD(std::unique_ptr<websocket_connection>, create_connection, (std::string url), (const, override));
	};

	class mock_websocket_connection : public websocket_connection
	{
	public:
		mock_websocket_connection()
			: websocket_connection{ std::weak_ptr<void>() }
		{}

		MOCK_METHOD(ws_connection_status, connection_status, (), (const, override));
		MOCK_METHOD(void, close, (), (override));
		MOCK_METHOD(void, send_message, (std::string message), (override));
	};

	class mock_back_testing_data_source : public back_testing_data_source
	{
	public:
		MOCK_METHOD(std::vector<tradable_pair>, get_available_pairs, (), (override));
		MOCK_METHOD(std::vector<ohlcv_data>, load_data, (const tradable_pair& pair, int stepSize), (override));
	};
}
