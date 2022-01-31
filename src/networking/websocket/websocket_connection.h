#pragma once

#include <memory>
#include <string>

#include "websocket_client.h"
#include "websocket_error.h"

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

    template<typename OnMessageHandler>
    websocket_connection create_websocket_connection(
        std::shared_ptr<websocket_client> client, 
        const std::string& url,
        OnMessageHandler onMessageHandler)
    {
        client::connection_ptr connectionPtr = client->get_connection(url);
        
        connectionPtr->set_message_handler(
            [onMessageHandler](websocketpp::connection_hdl, client::message_ptr message) { onMessageHandler(message->get_payload()); });

        client->connect(connectionPtr);

        return websocket_connection{ client, connectionPtr->get_handle() };
    }
}
