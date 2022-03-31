#include <memory>

#include "websocket_client.h"
#include "websocket_error.h"

namespace
{
    using namespace mb;

    std::shared_ptr<ssl_context> on_tls_init()
    {
        std::shared_ptr<ssl_context> context = std::make_shared<ssl_context>(ssl_context::sslv23);

        try
        {
            context->set_options(
                websocketpp::lib::asio::ssl::context::default_workarounds |
                websocketpp::lib::asio::ssl::context::no_sslv2 |
                websocketpp::lib::asio::ssl::context::no_sslv3 |
                websocketpp::lib::asio::ssl::context::single_dh_use);
        }
        catch (std::exception& e)
        {
            throw websocket_error{ std::format("Error occurred initialising TLS: {}", e.what()) };
        }

        return context;
    }
}

namespace mb
{
    websocket_client::websocket_client(int timeout)
        :_client{}, _thread{}
    {
        //_client.set_access_channels(websocketpp::log::alevel::none);
        _client.clear_access_channels(websocketpp::log::alevel::all);
        _client.clear_error_channels(websocketpp::log::elevel::all);

        _client.init_asio();
        _client.start_perpetual();
        _client.set_tls_init_handler(bind(&on_tls_init));
        _client.set_open_handshake_timeout(timeout);

        _thread = std::make_unique<std::thread>(&client::run, &_client);
    }

    websocket_client::~websocket_client()
    {
        _client.stop_perpetual();
        _thread->join();
    }

    void websocket_client::connect(client::connection_ptr connectionPtr)
    {
        try
        {
            _client.connect(connectionPtr);

            volatile bool connecting = true;
            while (connecting)
            {
                connecting = connectionPtr->get_state() == websocketpp::session::state::connecting;
            }
        }
        catch (const std::exception& e)
        {
            throw websocket_error{ std::format("Error occured connecting to websocket: {}", e.what()) };
        }
    }

    client::connection_ptr websocket_client::get_connection(std::string_view url)
    {
        std::error_code errorCode;
        auto connectionPtr = _client.get_connection(url.data(), errorCode);

        if (errorCode)
        {
            throw websocket_error{ std::format("Error occured getting connection for {0}: {1}", url, errorCode.message()) };
        }

        return connectionPtr;
    }

    client::connection_ptr websocket_client::get_connection(websocketpp::connection_hdl connectionHandle)
    {
        std::error_code errorCode;
        auto connectionPtr = _client.get_con_from_hdl(connectionHandle, errorCode);

        if (connectionPtr)
        {
            return connectionPtr;
        }

        throw websocket_error{ std::format("Error occurred getting connection: {}", errorCode.message()) };
    }

    void websocket_client::close_connection(websocketpp::connection_hdl connectionHandle)
    {
        std::error_code errorCode;
        auto connectionPtr = _client.get_con_from_hdl(connectionHandle, errorCode);

        if (connectionPtr)
        {
            connectionPtr->close(websocketpp::close::status::going_away, "", errorCode);
        }

        if (errorCode)
        {
            throw websocket_error{ std::format("Error occured closing connection: {}", errorCode.message()) };
        }
    }

    websocketpp::session::state::value websocket_client::get_state(websocketpp::connection_hdl connectionHandle)
    {
        std::error_code errorCode;
        auto connectionPtr = _client.get_con_from_hdl(connectionHandle, errorCode);
     
        if (connectionPtr)
        {
            return connectionPtr->get_state();
        }

        throw websocket_error{ std::format("Could not get connection from handle: {}", errorCode.message()) };
    }

    void websocket_client::send_message(websocketpp::connection_hdl connectionHandle, std::string_view message)
    {
        std::error_code errorCode;

        _client.send(connectionHandle, message.data(), websocketpp::frame::opcode::text, errorCode);

        if (errorCode)
        {
            throw websocket_error{ std::format("Error occurred sending message: {}", errorCode.message()) };
        }
    }
}