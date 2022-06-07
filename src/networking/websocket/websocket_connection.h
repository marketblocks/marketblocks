#pragma once

#include <memory>
#include <string>

#include "websocket_client.h"
#include "websocket_error.h"

namespace mb
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
        websocket_client& _client;
        websocketpp::connection_hdl _connectionHandle;

    public:
        websocket_connection(websocketpp::connection_hdl connectionHandle);

        virtual ~websocket_connection()
        {
            close();
        }

        websocket_connection(const websocket_connection&) = delete;
        websocket_connection(websocket_connection&&) noexcept = default;

        websocket_connection& operator=(const websocket_connection&) = delete;
        websocket_connection& operator=(websocket_connection&&) noexcept = default;

        ws_connection_status connection_status() const;
        void connect();
        void close();
        void send_message(std::string_view message);

        template<typename OnOpenHandler>
        void set_on_open_handler(OnOpenHandler handler)
        {
            _client.get_connection(_connectionHandle)->set_open_handler(
                [handler](websocketpp::connection_hdl)
                {
                    handler();
                });
        }

        template<typename OnCloseHandler>
        void set_on_close_handler(OnCloseHandler handler)
        {
            auto connection = _client.get_connection(_connectionHandle);

            connection->set_close_handler(
                [handler, connection](websocketpp::connection_hdl)
                {
                    handler(connection->get_ec());
                });
        }

        template<typename OnFailHandler>
        void set_on_fail_handler(OnFailHandler handler)
        {
            auto connection = _client.get_connection(_connectionHandle);

            connection->set_fail_handler(
                [handler, connection](websocketpp::connection_hdl)
                {
                    handler(connection->get_ec());
                });
        }

        template<typename OnMessageHandler>
        void set_on_message_handler(OnMessageHandler handler)
        {
            _client.get_connection(_connectionHandle)->set_message_handler(
                [handler](websocketpp::connection_hdl, client::message_ptr message)
                {
                    handler(message->get_payload());
                });
        }
    };
}
