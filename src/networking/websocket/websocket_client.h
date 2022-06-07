#pragma once

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

namespace mb
{
	typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
	typedef websocketpp::lib::asio::ssl::context ssl_context;

	class websocket_connection;

	class websocket_client
	{
	private:
		client _client;
		std::unique_ptr<std::thread> _thread;

		websocket_client();

	public:
		~websocket_client();

		websocket_client(const websocket_client&) = delete;
		websocket_client(websocket_client&&) noexcept = delete;
		websocket_client& operator=(const websocket_client&) = delete;
		websocket_client& operator=(websocket_client&&) noexcept = delete;

		static websocket_client& instance();

		void set_open_handshake_timeout(int timeout);
		websocket_connection create_connection(std::string_view url);

		void connect(client::connection_ptr connectionPtr);
		client::connection_ptr get_connection(std::string_view url);
		client::connection_ptr get_connection(websocketpp::connection_hdl connectionHandle);
		void close_connection(websocketpp::connection_hdl connectionHandle);
		websocketpp::session::state::value get_state(websocketpp::connection_hdl connectionHandle);
		void send_message(websocketpp::connection_hdl connectionHandle, std::string_view message);
	};
}