#pragma once

#include "exchanges/websockets/websocket_stream.h"

namespace cb
{
	class coinbase_websocket_stream : public websocket_stream
	{
	protected:
		std::string stream_url() const override { return ""; }

		void on_open() override;
		void on_close(std::string_view reason) override;
		void on_fail(std::string_view reason) override;
		void on_message(std::string_view message) override;

	public:
		coinbase_websocket_stream(std::shared_ptr<websocket_client> websocketClient);

		virtual void subscribe_order_book(const std::vector<tradable_pair>& tradablePairs) override;
	};
}