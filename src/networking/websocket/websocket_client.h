#pragma once

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> wsclient;
typedef websocketpp::lib::asio::ssl::context sslContext;

class WebsocketClient
{
private:
	wsclient _client;
	std::unique_ptr<std::thread> _thread;

public:
	WebsocketClient();
	~WebsocketClient();

	WebsocketClient(const WebsocketClient& other) = delete;
	WebsocketClient(WebsocketClient&& other) = default;

	WebsocketClient& operator=(const WebsocketClient& other) = delete;
	WebsocketClient& operator=(WebsocketClient&& other) = default;

	wsclient::connection_ptr get_connection(const std::string& url, std::error_code errorCode)
	{
		return _client.get_connection(url, errorCode);
	}

	void connect(wsclient::connection_ptr connectionPtr)
	{
		_client.connect(connectionPtr);
	}

	void close_connection(websocketpp::connection_hdl connectionHandle)
	{
		_client.get_con_from_hdl(connectionHandle)->close(websocketpp::close::status::going_away, "");
	}

	websocketpp::session::state::value get_state(websocketpp::connection_hdl connectionHandle)
	{
		return _client.get_con_from_hdl(connectionHandle)->get_state();
	}

	void send_message(websocketpp::connection_hdl connectionHandle, const std::string& message)
	{
		_client.send(connectionHandle, message, websocketpp::frame::opcode::text);
	}
};