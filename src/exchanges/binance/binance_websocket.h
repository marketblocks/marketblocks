#pragma once

#include "common/json/json.h"
#include "exchanges/exchange.h"/
#include "exchanges/websockets/exchange_websocket_stream.h"

namespace mb::internal
{
	class binance_websocket_stream : public exchange_websocket_stream
	{
	private:
		std::unique_ptr<market_api> _marketApi;
		std::unordered_map<std::string, std::time_t> _orderBookIds;

		void process_trade_message(const json_document& json);
		void process_ohlcv_message(const json_document& json);
		void process_order_book_message(const json_document& json);
		void process_order_book_element(order_book_side side, std::string_view pair, std::time_t id, const json_element& element);

		void on_message(std::string_view message) override;
		void send_subscribe(const websocket_subscription& subscription) override;
		void send_unsubscribe(const websocket_subscription& subscription) override;

	public:
		binance_websocket_stream(
			std::unique_ptr<websocket_connection_factory> connectionFactory,
			std::unique_ptr<market_api> marketApi);
	};
}