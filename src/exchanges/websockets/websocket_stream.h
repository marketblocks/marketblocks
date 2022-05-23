#pragma once

#include "websocket_stream_implementation.h"
#include "networking/websocket/websocket_connection.h"
#include "exchanges/exchange_status.h"
#include "trading/tradable_pair.h"

namespace mb
{
	class websocket_stream
	{
	public:
		virtual ~websocket_stream() = default;

		virtual void connect() = 0;
		virtual ws_connection_status connection_status() const = 0;
		virtual void subscribe_order_book(const std::vector<tradable_pair>& tradablePairs) = 0;
		virtual void unsubscribe_order_book(const std::vector<tradable_pair>& tradablePairs) = 0;
		virtual bool is_order_book_subscribed(const tradable_pair& pair) const = 0;
		virtual order_book_state get_order_book(const tradable_pair& pair, int depth = 0) const = 0;
		virtual set_queue<tradable_pair>& get_order_book_message_queue() = 0;
	};

	class exchange_websocket_stream : public websocket_stream
	{
	private:
		std::unique_ptr<internal::websocket_stream_implementation> _implementation;
		std::shared_ptr<websocket_client> _websocketClient;
		std::unique_ptr<websocket_connection> _connection;

		void send_message(std::string_view message) const;

	public:
		exchange_websocket_stream(
			std::unique_ptr<internal::websocket_stream_implementation> implementation,
			std::shared_ptr<websocket_client> websocketClient);

		void connect() override;
		ws_connection_status connection_status() const override;
		void subscribe_order_book(const std::vector<tradable_pair>& tradablePairs) override;
		void unsubscribe_order_book(const std::vector<tradable_pair>& tradablePairs) override;
		
		bool is_order_book_subscribed(const tradable_pair& pair) const override
		{
			return _implementation->is_order_book_subscribed(pair);
		}

		order_book_state get_order_book(const tradable_pair& pair, int depth = 0) const override
		{ 
			return _implementation->get_order_book(pair, depth);
		}

		set_queue<tradable_pair>& get_order_book_message_queue() noexcept override
		{ 
			return _implementation->get_order_book_message_queue();
		}
	};
}