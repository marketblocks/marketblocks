#pragma once

#include <memory>
#include <string>

#include "websocket_client.h"
#include "websocket_error.h"

namespace mb
{
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

        virtual ws_connection_status connection_status() const;
        virtual void close();
        virtual void send_message(std::string_view message);
    };

    class websocket_connection_factory
    {
    private:
        using on_open = std::function<void()>;
        using on_close = std::function<void()>;
        using on_message = std::function<void(std::string_view)>;

        on_open _onOpen;
        on_close _onClose;
        on_message _onMessage;

    public:
        virtual ~websocket_connection_factory() = default;

        void set_on_open(on_open onOpen) noexcept { _onOpen = std::move(onOpen); }
        void set_on_close(on_close onClose) noexcept { _onClose = std::move(onClose); }
        void set_on_message(on_message onMessage) noexcept { _onMessage = std::move(onMessage); }

        virtual std::unique_ptr<websocket_connection> create_connection(std::string url) const;
    };
}