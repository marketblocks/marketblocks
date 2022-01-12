#include "websocket_connection.h"

namespace
{
    WsConnectionStatus map_connection_status(websocketpp::session::state::value state)
    {
        switch (state)
        {
        case websocketpp::session::state::closed:
            return WsConnectionStatus::CLOSED;
        case websocketpp::session::state::closing:
            return WsConnectionStatus::CLOSING;
        case websocketpp::session::state::connecting:
            return WsConnectionStatus::CONNECTING;
        case websocketpp::session::state::open:
            return WsConnectionStatus::OPEN;
        }
    }
}

WebsocketConnection::WebsocketConnection(std::shared_ptr<WebsocketClient> client, websocketpp::connection_hdl connectionHandle)
    : _client { client }, _connectionHandle{std::move(connectionHandle)}
{
}

WebsocketConnection::~WebsocketConnection()
{
    if (!_connectionHandle.expired())
    {
        _client->close_connection(_connectionHandle);
    }
}

void WebsocketConnection::send_message(const std::string& message)
{
    _client->send_message(_connectionHandle, message);
}

WsConnectionStatus WebsocketConnection::connection_status() const
{
    return map_connection_status(_client->get_state(_connectionHandle));
}

WebsocketConnection create_websocket_connection(std::shared_ptr<WebsocketClient> client, const std::string& url, const WebsocketEventHandlers& eventHandlers)
{
    std::error_code ec;
    wsclient::connection_ptr connectionPtr = client->get_connection(url, ec);
    if (ec)
    {
        //std::cout << "could not create connection because: " << ec.message() << std::endl;
        throw std::runtime_error("Could not create connection: " + ec.message());
    }

    connectionPtr->set_message_handler(
        [eventHandlers]
    (websocketpp::connection_hdl, wsclient::message_ptr message) { eventHandlers.onMessage(message->get_payload()); });

    client->connect(connectionPtr);

    while (client->get_state(connectionPtr->get_handle()) == websocketpp::session::state::connecting)
    {
        
    }

    return WebsocketConnection{ client, connectionPtr->get_handle() };
}