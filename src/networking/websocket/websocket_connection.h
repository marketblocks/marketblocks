#pragma once

#include <memory>
#include <string>

#include "websocket_client.h"

enum class WsConnectionStatus
{
    CLOSED,
    CLOSING,
    CONNECTING,
    OPEN
};

class WebsocketConnection
{
private:
    std::shared_ptr<WebsocketClient> _client;
    websocketpp::connection_hdl _connectionHandle;

public:
    WebsocketConnection(std::shared_ptr<WebsocketClient> client, websocketpp::connection_hdl connectionHandle);
    ~WebsocketConnection();

    WebsocketConnection(const WebsocketConnection& other) = delete;
    WebsocketConnection(WebsocketConnection&& other) = default;

    WebsocketConnection& operator=(const WebsocketConnection& other) = delete;
    WebsocketConnection& operator=(WebsocketConnection&& other) = default;

    void send_message(const std::string& message);

    WsConnectionStatus connection_status() const;
};

struct WebsocketEventHandlers
{
public:
    std::function<void(const std::string&)> onMessage;
};

WebsocketConnection create_websocket_connection(std::shared_ptr<WebsocketClient> client, const std::string& url, const WebsocketEventHandlers& eventHandlers);