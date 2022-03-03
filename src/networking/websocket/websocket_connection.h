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

        void send_message(std::string_view message);

        ws_connection_status connection_status() const;
    };

    template<typename OnOpenHandler, typename OnCloseHandler, typename OnFailHandler, typename OnMessageHandler>
    websocket_connection create_websocket_connection(
        std::shared_ptr<websocket_client> client, 
        std::string_view url,
        OnOpenHandler onOpenHandler,
        OnCloseHandler onCloseHandler,
        OnFailHandler onFailHandler,
        OnMessageHandler onMessageHandler)
    {
        client::connection_ptr connectionPtr = client->get_connection(url);
        
        connectionPtr->set_open_handler(
            [onOpenHandler](websocketpp::connection_hdl) 
            { 
                onOpenHandler();
            });

        connectionPtr->set_close_handler(
            [onCloseHandler, client](websocketpp::connection_hdl handle) 
            { 
                std::string reason = client->get_connection(handle)->get_ec().message();
                onCloseHandler(reason);
            });

        connectionPtr->set_fail_handler(
            [onFailHandler, client](websocketpp::connection_hdl handle) 
            { 
                std::string reason = client->get_connection(handle)->get_ec().message();
                onFailHandler(reason);
            });

        connectionPtr->set_message_handler(
            [onMessageHandler](websocketpp::connection_hdl, client::message_ptr message) 
            { 
                onMessageHandler(message->get_payload());
            });

        //connectionPtr->set_open_handshake_timeout(0);

        client->connect(connectionPtr);

        return websocket_connection{ client, connectionPtr->get_handle() };
    }
}
