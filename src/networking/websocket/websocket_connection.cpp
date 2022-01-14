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

    void websocket_connection::send_message(const std::string& message)
    {
        _client->send_message(_connectionHandle, message);
    }

    ws_connection_status websocket_connection::connection_status() const
    {
        return map_connection_status(_client->get_state(_connectionHandle));
    }

    websocket_connection create_websocket_connection(std::shared_ptr<websocket_client> client, const std::string& url, const websocket_event_handlers& eventHandlers)
    {
        std::error_code ec;
        client::connection_ptr connectionPtr = client->get_connection(url, ec);
        if (ec)
        {
            //std::cout << "could not create connection because: " << ec.message() << std::endl;
            throw std::runtime_error("Could not create connection: " + ec.message());
        }

        connectionPtr->set_message_handler(
            [eventHandlers]
        (websocketpp::connection_hdl, client::message_ptr message) { eventHandlers.onMessage(message->get_payload()); });

        client->connect(connectionPtr);

        while (client->get_state(connectionPtr->get_handle()) == websocketpp::session::state::connecting)
        {

        }

        return websocket_connection{ client, connectionPtr->get_handle() };
    }
}