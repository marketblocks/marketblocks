#pragma once

#include "order_book_cache.h"
#include "networking/websocket/websocket_connection.h"
#include "trading/tradable_pair.h"

class WebsocketStream
{
private:
	std::unique_ptr<WebsocketConnection> _connection;

	virtual std::string stream_url() const = 0;
	virtual void onMessage(const std::string& message) = 0;

protected:
	void send_message(const std::string& message)
	{
		_connection->send_message(message);
	}

public:
	void connect(std::shared_ptr<WebsocketClient> websocketClient);
	WsConnectionStatus connection_status() const;

	virtual void subscribe_order_book(const std::vector<TradablePair>& tradablePairs) = 0;
	virtual OrderBookState get_current_order_book(const TradablePair& tradablePair) const = 0;
};