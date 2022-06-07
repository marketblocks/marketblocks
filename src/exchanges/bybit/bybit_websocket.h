#pragma once

#include "exchanges/websockets/exchange_websocket_stream.h"

namespace mb::internal
{
	class bybit_websocket_stream : public exchange_websocket_stream
	{
	private:
		std::unordered_map<std::string, double> _prices;
		std::unordered_map<std::string, ohlcv_data> _ohlcvData;

		void on_message(std::string_view message) override;

	public:
		bybit_websocket_stream(std::unique_ptr<websocket_connection> connection);

		static std::string url() noexcept { return "wss://stream.bybit.com/spot/quote/ws/v1"; }

		void subscribe(const websocket_subscription& subscription) override;
		void unsubscribe(const websocket_subscription& subscription) override;
		bool is_subscribed(const websocket_subscription& subscription) override;
	};
}