#pragma once

#include <unordered_map>

#include "websocket_stream.h"
#include "order_book_cache.h"
#include "common/types/concurrent_wrapper.h"

#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	class exchange_websocket_stream : public websocket_stream
	{
	private:
		std::unique_ptr<websocket_connection_factory> _connectionFactory;

		std::string_view _id;
		std::string _url;
		char _pairSeparator;

		concurrent_wrapper<std::unordered_map<std::string, tradable_pair>> _pairs;
		concurrent_wrapper<std::unordered_map<std::string, trade_update>> _trades;
		concurrent_wrapper<std::unordered_map<std::string, ohlcv_data>> _ohlcv;
		concurrent_wrapper<std::unordered_map<std::string, order_book_cache>> _orderBooks;

		void initialise_connection_factory();
		void clear_subscriptions();

		void on_open();
		void on_close();

		virtual void on_message(std::string_view message) = 0;
		virtual void send_subscribe(const websocket_subscription& subscription) = 0;
		virtual void send_unsubscribe(const websocket_subscription& subscription) = 0;

	protected:
		std::unique_ptr<websocket_connection> _connection;

		void set_unsubscribed(const named_subscription& subscription);
		void update_trade(std::string pairName, trade_update trade);
		void update_ohlcv(std::string pairName, ohlcv_interval interval, ohlcv_data ohlcvData);
		void initialise_order_book(std::string pairName, order_book_cache cache);
		void update_order_book(std::string pairName, order_book_entry entry);

	public:
		exchange_websocket_stream(
			std::string_view id, 
			std::string url, 
			char pairSeparator,
			std::unique_ptr<websocket_connection_factory> connectionFactory);

		virtual ~exchange_websocket_stream() = default;

		std::string_view id() const noexcept { return _id; }

		void reset() override;
		void disconnect() override;
		ws_connection_status connection_status() const override;

		void subscribe(const websocket_subscription& subscription) override;
		void unsubscribe(const websocket_subscription& subscription) override;
		subscription_status get_subscription_status(const unique_websocket_subscription& subscription) const override;

		order_book_state get_order_book(const tradable_pair& pair, int depth = 0) const override;
		trade_update get_last_trade(const tradable_pair& pair) const override;
		ohlcv_data get_last_candle(const tradable_pair& pair, ohlcv_interval interval) const override;
	};

	template<typename Implementation>
	std::unique_ptr<websocket_stream> create_exchange_websocket_stream()
	{
		return std::make_unique<Implementation>(
			std::make_unique<websocket_connection_factory>());
	}
}
