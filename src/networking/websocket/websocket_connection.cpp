#include "websocket_connection.h"

namespace mb
{
    websocket_connection::websocket_connection(websocketpp::connection_hdl connectionHandle)
        : 
        _client{ websocket_client::instance() }, 
        _connectionHandle{ connectionHandle }
    {}

    void websocket_connection::close()
    {
        _client.close_connection(_connectionHandle);
    }

    void websocket_connection::send_message(std::string_view message)
    {
        _client.send_message(_connectionHandle, message);
    }

    ws_connection_status websocket_connection::connection_status() const
    {
        return _client.get_connection_status(_connectionHandle);
    }

    std::unique_ptr<websocket_connection> websocket_connection_factory::create_connection(std::string url) const
    {
        auto handle =  websocket_client::instance().create_connection(url, _onOpen, _onClose, _onFail, _onMessage);
        return std::make_unique<websocket_connection>(handle);
    }
}