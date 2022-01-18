#pragma once

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

namespace cb
{
	typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
	typedef websocketpp::lib::asio::ssl::context ssl_context;

	class websocket_client
	{
	private:
		client _client;
		std::unique_ptr<std::thread> _thread;

	public:
		websocket_client();
		~websocket_client();

		websocket_client(const websocket_client& other) = delete;
		websocket_client(websocket_client&& other) noexcept = default;

		websocket_client& operator=(const websocket_client& other) = delete;
		websocket_client& operator=(websocket_client&& other) noexcept = default;

		client::connection_ptr get_connection(const std::string& url, std::error_code errorCode)
		{
			return _client.get_connection(url, errorCode);
		}

		void connect(client::connection_ptr connectionPtr)
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
}