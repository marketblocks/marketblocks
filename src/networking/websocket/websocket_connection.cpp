#include "websocket_connection.h"

namespace
{
    using namespace mb;

    ws_connection_status map_connection_status(websocketpp::session::state::value state)
    {
        switch (state)
        {
        case websocketpp::session::state::closed:
            return ws_connection_status::CLOSED;
        case websocketpp::session::state::closing:
            return ws_connection_status::CLOSING;
        case websocketpp::session::state::connecting:
            return ws_connection_status::CONNECTING;
        case websocketpp::session::state::open:
            return ws_connection_status::OPEN;
        }
    }
}

namespace mb
{
    websocket_connection::websocket_connection(websocketpp::connection_hdl connectionHandle)
        : 
        _client{ websocket_client::instance() }, 
        _connectionHandle{std::move(connectionHandle)}
    {}

    void websocket_connection::connect()
    {
        _client.connect(_client.get_connection(_connectionHandle));
    }

    void websocket_connection::close()
    {
        if (!_connectionHandle.expired())
        {
            _client.close_connection(_connectionHandle);
        }
    }

    void websocket_connection::send_message(std::string_view message)
    {
        _client.send_message(_connectionHandle, message);
    }

    ws_connection_status websocket_connection::connection_status() const
    {
        return map_connection_status(_client.get_state(_connectionHandle));
    }
}