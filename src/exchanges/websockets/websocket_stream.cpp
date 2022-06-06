#include "websocket_stream.h"
#include "logging/logger.h"

namespace
{
	#define CB_ASSERT_CONNECTION_EXISTS(connection) (assert(connection, "Connection has not been created"))
}

namespace mb
{
	exchange_websocket_stream::exchange_websocket_stream(
		std::unique_ptr<internal::websocket_stream_implementation> implementation,
		std::shared_ptr<websocket_client> websocketClient)
		: 
		_implementation{ std::move(implementation) },
		_websocketClient{ websocketClient }, 
		_connection{}
	{}

	void exchange_websocket_stream::connect()
	{
		if (connection_status() == ws_connection_status::OPEN)
		{
			return;
		}

		_connection = std::make_unique<websocket_connection>(
			create_websocket_connection(
				_websocketClient,
				_implementation->stream_url(),
				[this]() { _implementation->on_open(); },
				[this](std::error_code reason) { _implementation->on_close(reason); },
				[this](std::error_code reason) { _implementation->on_fail(reason); },
				[this](const std::string& message) { _implementation->on_message(message); }));
	}

	ws_connection_status exchange_websocket_stream::connection_status() const
	{
		if (_connection)
		{
			return _connection->connection_status();
		}

		return ws_connection_status::CLOSED;
	}

	void exchange_websocket_stream::send_message(std::string_view message) const
	{
		CB_ASSERT_CONNECTION_EXISTS(_connection);

		_connection->send_message(message);
	}

	void exchange_websocket_stream::subscribe_order_book(const std::vector<tradable_pair>& tradablePairs)
	{
		assert(tradablePairs.size() > 0);

		std::string subscriptionMessage{ _implementation->get_order_book_subscription_message(tradablePairs) };
		send_message(subscriptionMessage);
	}

	void exchange_websocket_stream::unsubscribe_order_book(const std::vector<tradable_pair>& tradablePairs)
	{
		assert(tradablePairs.size() > 0);

		std::string unsubscriptionMessage{ _implementation->get_order_book_unsubscription_message(tradablePairs) };
		send_message(unsubscriptionMessage);
	}

	void exchange_websocket_stream::subscribe_price(const std::vector<tradable_pair>& tradablePairs)
	{
		std::string subscriptionMessage{ _implementation->get_price_subscription_message(tradablePairs) };
		send_message(subscriptionMessage);
	}

	void exchange_websocket_stream::unsubscribe_price(const std::vector<tradable_pair>& tradablePairs)
	{
		std::string unsubscriptionMessage{ _implementation->get_price_unsubscription_message(tradablePairs) };
		send_message(unsubscriptionMessage);
	}

	void exchange_websocket_stream::subscribe_candles(const std::vector<tradable_pair>& tradablePairs, int interval)
	{
		std::string subscriptionMessage{ _implementation->get_candles_subscription_message(tradablePairs, interval) };
		send_message(subscriptionMessage);
	}

	void exchange_websocket_stream::unsubscribe_candles(const std::vector<tradable_pair>& tradablePairs, int interval)
	{
		std::string unsubscriptionMessage{ _implementation->get_candles_unsubscription_message(tradablePairs, interval) };
		send_message(unsubscriptionMessage);
	}
}