#pragma once

#include "exchanges/websockets/websocket_stream.h"
#include "common/file/json_wrapper.h"

namespace cb
{
	class kraken_websocket_stream : public websocket_stream
	{
	private:
		void on_message(const std::string& message) override;

		void process_order_book_message(const rapidjson::GenericArray<false, rapidjson::Value>& messageObject);
		void process_order_book_object(const std::string& pair, const rapidjson::GenericObject<false, rapidjson::Value>& object);

	protected:
		std::string stream_url() const override { return "wss://ws.kraken.com"; }

		std::string get_subscribe_order_book_message(const std::vector<tradable_pair>& tradablePairs) const override;
	};
}