#include "websocket_connection.h"

namespace
{
    cb::ws_connection_status map_connection_status(websocketpp::session::state::value state)
    {
        switch (state)
        {
        case websocketpp::session::state::closed:
            return cb::ws_connection_status::CLOSED;
        case websocketpp::session::state::closing:
            return cb::ws_connection_status::CLOSING;
        case websocketpp::session::state::connecting:
            return cb::ws_connection_status::CONNECTING;
        case websocketpp::session::state::open:
            return cb::ws_connection_status::OPEN;
        }
    }
}

namespace cb
{
    websocket_connection::websocket_connection(std::shared_ptr<websocket_client> client, websocketpp::connection_hdl connectionHandle)
        : _client{ client }, _connectionHandle{ std::move(connectionHandle) }
    {
    }

    websocket_connection::~websocket_connection()
    {
        if (!_connectionHandle.expired())
        {
            _client->close_connection(_connectionHandle);
        }
    }

    void websocket_connection::send_message(std::string_view message)
    {
        _client->send_message(_connectionHandle, message);
    }

    ws_connection_status websocket_connection::connection_status() const
    {
        return map_connection_status(_client->get_state(_connectionHandle));
    }
}