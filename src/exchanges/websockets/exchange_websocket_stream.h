#pragma once

#include "websocket_stream.h"
#include "order_book_cache.h"
#include "common/types/unordered_string_map.h"

#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	class exchange_websocket_stream : public websocket_stream
	{
	private:
		std::unique_ptr<websocket_connection_factory> _connectionFactory;

		std::string_view _id;
		std::string_view _url;
		unordered_string_map<double> _prices;
		unordered_string_map<ohlcv_data> _ohlcv;
		unordered_string_map<order_book_cache> _orderBooks;
		set_queue<tradable_pair> _messageQueue;

		void initialise_connection_factory();

		void on_open() const;
		void on_close(std::error_code error) const;
		void on_fail(std::error_code error) const;

		virtual void on_message(std::string_view message) = 0;
		virtual std::string generate_subscription_id(const unique_websocket_subscription& subscription) const = 0;

	protected:
		std::unique_ptr<websocket_connection> _connection;

		void update_price(std::string subscriptionId, double price);
		void update_ohlcv(std::string subscriptionId, ohlcv_data ohlcvData);
		void initialise_order_book(std::string subscriptionId, order_book_cache cache);
		void update_order_book(std::string subscriptionId, order_book_entry entry);

	public:
		exchange_websocket_stream(
			std::string_view id, 
			std::string_view url, 
			std::unique_ptr<websocket_connection_factory> connectionFactory);

		void reset() override;
		void disconnect() override;
		ws_connection_status connection_status() const override;

		order_book_state get_order_book(const tradable_pair& pair, int depth = 0) const override;
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
		return std::make_unique<Implementation>(
			std::make_unique<websocket_connection_factory>());
	}
}