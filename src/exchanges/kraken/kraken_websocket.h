#pragma once

#include "exchanges/websockets/exchange_websocket_stream.h"
#include "common/json/json.h"

namespace mb::internal
{
	class kraken_websocket_stream : public exchange_websocket_stream
	{
	private:
		void process_event_message(const json_document& json);
		void process_trade_message(std::string pairName, const json_document& json);
		void process_ohlcv_message(std::string pairName, std::string channelName, const json_document& json);
		void process_order_book_message(std::string pairName, const json_document& json);
		void process_order_book_updates(std::string pairName, const json_element& updateElement);

		void on_message(std::string_view message) override;
		void send_subscribe(const websocket_subscription& subscription) override;
		void send_unsubscribe(const websocket_subscription& subscription) override;

	public:
		kraken_websocket_stream(std::unique_ptr<websocket_connection_factory> connectionFactory);
	};
}