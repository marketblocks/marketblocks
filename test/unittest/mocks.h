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
		MOCK_METHOD(double, get_price, (const tradable_pair& pair), (const, override));
		MOCK_METHOD(ohlcv_data, get_last_candle, (const tradable_pair& pair, ohlcv_interval interval), (const, override));

		MOCK_METHOD(set_queue<tradable_pair>&, get_order_book_message_queue, (), (noexcept, override));
	};

	class mock_exchange_websocket_stream : public exchange_websocket_stream
	{
	public:
		mock_exchange_websocket_stream(
			std::string_view id,
			std::string_view url,
			std::unique_ptr<websocket_connection_factory> connectionFactory)
			: exchange_websocket_stream{ id, url, std::move(connectionFactory) }
		{}

		MOCK_METHOD(void, on_message, (std::string_view message), (override));
		MOCK_METHOD(std::string, generate_subscription_id, (const unique_websocket_subscription& subscription), (const, override));
		MOCK_METHOD(void, subscribe, (const websocket_subscription& subscription), (override));
		MOCK_METHOD(void, unsubscribe, (const websocket_subscription& subscription), (override));

		void expose_update_price(std::string subId, double price)
		{
			update_price(std::move(subId), price);
		}

		void expose_update_ohlcv(std::string subId, ohlcv_data data)
		{
			update_ohlcv(std::move(subId), std::move(data));
		}

		void expose_initialise_order_book(std::string subId, order_book_cache cache)
		{
			initialise_order_book(std::move(subId), std::move(cache));
		}

		void expose_update_order_book(std::string subId, order_book_entry entry)
		{
			update_order_book(std::move(subId), std::move(entry));
		}

		void expose_set_unsubscribed(std::string subscriptionId, websocket_channel channel)
		{
			set_unsubscribed(std::move(subscriptionId), channel);
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
