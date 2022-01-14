#include "websocket_client.h"

#include <memory>

static std::shared_ptr<cb::ssl_context> on_tls_init()
{
    std::shared_ptr<cb::ssl_context> context = std::make_shared<cb::ssl_context>(cb::ssl_context::sslv23);

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
        std::cout << "Error in context pointer: " << e.what() << std::endl;
    }

    return context;
}

namespace cb
{
    websocket_client::websocket_client()
        :_client{}, _thread{}
    {
        _client.set_access_channels(websocketpp::log::alevel::all);
        _client.clear_access_channels(websocketpp::log::alevel::frame_payload);

        _client.init_asio();
        _client.start_perpetual();
        _client.set_tls_init_handler(bind(&on_tls_init));

        _thread = std::make_unique<std::thread>(&client::run, &_client);
    }

    websocket_client::~websocket_client()
    {
        _client.stop_perpetual();
        _thread->join();
    }
}