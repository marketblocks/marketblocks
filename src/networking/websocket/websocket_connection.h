#pragma once

#include <memory>
#include <string>

#include "websocket_client.h"

namespace cb
{
    enum class ws_connection_status
    {
        CLOSED,
        CLOSING,
        CONNECTING,
        OPEN
    };

    class websocket_connection
    {
    private:
        std::shared_ptr<websocket_client> _client;
        websocketpp::connection_hdl _connectionHandle;

    public:
        websocket_connection(std::shared_ptr<websocket_client> client, websocketpp::connection_hdl connectionHandle);
        ~websocket_connection();

        websocket_connection(const websocket_connection& other) = delete;
        websocket_connection(websocket_connection&& other) noexcept = default;

        websocket_connection& operator=(const websocket_connection& other) = delete;
        websocket_connection& operator=(websocket_connection&& other) noexcept = default;

        void send_message(const std::string& message);

        ws_connection_status connection_status() const;
    };

    struct websocket_event_handlers
    {
    public:
        std::function<void(const std::string&)> onMessage;
    };

    websocket_connection create_websocket_connection(std::shared_ptr<websocket_client> client, const std::string& url, const websocket_event_handlers& eventHandlers);
}
