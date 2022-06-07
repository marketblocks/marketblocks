#pragma once

#include "websocket_stream.h"

namespace mb
{
	class exchange_websocket_stream : public websocket_stream
	{
	private:
		set_queue<tradable_pair> _messageQueue;
		std::string_view _id;

		void on_open() const;
		void on_close(std::error_code error) const;
		void on_fail(std::error_code error) const;
		virtual void on_message(std::string_view message) = 0;

	protected:
		std::unique_ptr<websocket_connection> _connection;

	public:
		exchange_websocket_stream(std::string_view id, std::unique_ptr<websocket_connection> connection);

		void connect() override
		{
			_connection->connect();
		}

		void disconnect() override
		{
			_connection->close();
		}

		ws_connection_status connection_status() const override
		{
			return _connection->connection_status();
		}

		order_book_state get_order_book(const tradable_pair& pair, order_book_depth depth) const override;
		double get_price(const tradable_pair& pair) const override;
		ohlcv_data get_last_candle(const tradable_pair& pair, ohlcv_interval interval) const override;

		set_queue<tradable_pair>& get_order_book_message_queue() override
		{
			return _messageQueue;
		}
	};

	template<typename Implementation>
	std::unique_ptr<websocket_stream> create_exchange_websocket_stream()
	{
		std::unique_ptr<websocket_connection> connection
		{ 
			std::make_unique<websocket_connection>(websocket_client::instance().create_connection(Implementation::url())) 
		};

		return std::make_unique<Implementation>(std::move(connection));
	}
}