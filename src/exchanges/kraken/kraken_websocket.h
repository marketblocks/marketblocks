#pragma once

#include "exchanges/websockets/websocket_stream_implementation.h"
#include "common/file/json.h"

namespace cb::internal
{
	class kraken_websocket_stream : public websocket_stream_implementation
	{
	private:
		void process_order_book_message(const json_document& json);
		void process_order_book_object(const tradable_pair& pair, const json_element& json);
		void process_order_book_initialisation(const tradable_pair& pair, const json_element& json);

		void process_event_message(const json_document& json);
		void process_update_message(const json_document& json);

	public:
		std::string stream_url() const noexcept override { return "wss://ws.kraken.com"; }

		void on_open() override;
		void on_close(std::string_view reason) override;
		void on_fail(std::string_view reason) override;
		void on_message(std::string_view message) override;

		std::string get_order_book_subscription_message(const std::vector<tradable_pair>& tradablePairs) const override;
	};
}