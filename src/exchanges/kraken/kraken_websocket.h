#pragma once

#include "exchanges/websockets/websocket_stream.h"
#include "common/file/json_wrapper.h"

class KrakenWebsocketStream : public WebsocketStream
{
private:
	std::unordered_map<std::string, OrderBookCache> _orderBookCaches;

	std::string stream_url() const override { return "wss://ws.kraken.com"; }

	void onMessage(const std::string& message) override;

	void process_order_book_message(const rapidjson::GenericArray<false, rapidjson::Value>& messageObject);
	void process_order_book_object(const std::string& pair, const rapidjson::GenericObject<false, rapidjson::Value>& object);

public:
	void subscribe_order_book(const std::vector<TradablePair>& tradablePairs) override;
	OrderBookState get_current_order_book(const TradablePair& tradablePair) const override;
};