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

		void connect(client::connection_ptr connectionPtr);
		client::connection_ptr get_connection(const std::string& url);
		void close_connection(websocketpp::connection_hdl connectionHandle);
		websocketpp::session::state::value get_state(websocketpp::connection_hdl connectionHandle);
		void send_message(websocketpp::connection_hdl connectionHandle, const std::string& message);
	};
}