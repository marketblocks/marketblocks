#pragma once

#include "order_book_cache.h"
#include "networking/websocket/websocket_connection.h"
#include "trading/tradable_pair.h"

class WebsocketStream
{
private:
	std::unique_ptr<WebsocketConnection> _connection;

protected:
	std::unordered_map<std::string, OrderBookCache> _orderBookCaches;

	virtual std::string stream_url() const = 0;
	virtual std::string get_subscribe_order_book_message(const std::vector<TradablePair>& tradablePairs) const = 0;
	
	virtual void on_message(const std::string& message) = 0;

public:
	void connect(std::shared_ptr<WebsocketClient> websocketClient);

	WsConnectionStatus connection_status() const;

	void subscribe_order_book(const std::vector<TradablePair>& tradablePairs);
	OrderBookState get_order_book_snapshot(const TradablePair& tradablePair) const;
};