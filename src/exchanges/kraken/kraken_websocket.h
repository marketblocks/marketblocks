#pragma once

#include "exchanges/websockets/websocket_stream.h"
#include "common/file/json.h"

namespace cb
{
	class kraken_websocket_stream : public websocket_stream
	{
	private:
		void process_order_book_message(const json_document& json);
		void process_order_book_object(const std::string& pair, const json_element& json);
		void process_order_book_initialisation(const std::string& pair, const json_element& json);

		void process_event_message(const json_document& json);
		void process_update_message(const json_document& json);

	protected:
		std::string stream_url() const override { return "wss://ws.kraken.com"; }

		void on_open() override;
		void on_close(const std::string& reason) override;
		void on_fail(const std::string& reason) override;
		void on_message(const std::string& message) override;

	public:
		kraken_websocket_stream(std::shared_ptr<websocket_client> websocketClient);

		void subscribe_order_book(const std::vector<tradable_pair>& tradablePairs) override;
	};
}