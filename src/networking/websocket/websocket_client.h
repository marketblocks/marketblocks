#pragma once

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include "websocket_error.h"
#include "websocket_constants.h"

namespace mb
{
	typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
	typedef websocketpp::lib::asio::ssl::context ssl_context;

	class websocket_client
	{
	private:
		client _client;
		std::unique_ptr<std::thread> _thread;

		websocket_client();

		void connect(client::connection_ptr connectionPtr);

	public:
		~websocket_client();

		websocket_client(const websocket_client&) = delete;
		websocket_client(websocket_client&&) noexcept = delete;
		websocket_client& operator=(const websocket_client&) = delete;
		websocket_client& operator=(websocket_client&&) noexcept = delete;

		static websocket_client& instance();

		void set_open_handshake_timeout(int timeout);

		template<typename OnOpen, typename OnClose,	typename OnFail, typename OnMessage>
		websocketpp::connection_hdl create_connection(
			std::string_view url,
			OnOpen onOpen,
			OnClose onClose,
			OnFail onFail,
			OnMessage onMessage)
		{
			std::error_code errorCode;
			auto connectionPtr = _client.get_connection(url.data(), errorCode);

			if (errorCode)
			{
				throw websocket_error{ std::format("Getting connection for {0}: {1}", url, errorCode.message()) };
			}

			connectionPtr->set_open_handler(
				[onOpen](websocketpp::connection_hdl)
				{
					onOpen();
				});

			connectionPtr->set_close_handler(
				[onClose, this](websocketpp::connection_hdl handle)
				{
					auto connection = _client.get_con_from_hdl(handle);
					onClose(connection->get_ec());
				});

			connectionPtr->set_fail_handler(
				[onFail, this](websocketpp::connection_hdl handle)
				{
					auto connection = _client.get_con_from_hdl(handle);
					onFail(connection->get_ec());
				});

			connectionPtr->set_message_handler(
				[onMessage](websocketpp::connection_hdl, client::message_ptr message)
				{
					onMessage(message->get_payload());
				});

			connect(connectionPtr);

			return connectionPtr->get_handle();
		}
				
		void close_connection(websocketpp::connection_hdl connectionHandle);
		ws_connection_status get_connection_status(websocketpp::connection_hdl connectionHandle);
		void send_message(websocketpp::connection_hdl connectionHandle, std::string_view message);
	};
}