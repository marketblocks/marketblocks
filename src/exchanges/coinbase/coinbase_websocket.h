#pragma once

#include "exchanges/websockets/websocket_stream_implementation.h"

namespace cb::internal
{
	class coinbase_websocket_stream : public websocket_stream_implementation
	{
	public:
		std::string stream_url() const noexcept override { return ""; }

		void on_open() override;
		void on_close(std::string_view reason) override;
		void on_fail(std::string_view reason) override;
		void on_message(std::string_view message) override;

		std::string get_order_book_subscription_message(const std::vector<tradable_pair>& tradablePairs) const override;
	};
}