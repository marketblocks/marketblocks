#pragma once

#include "exchanges/websockets/websocket_stream_implementation.h"

namespace mb::internal
{
	class bybit_websocket_stream : public websocket_stream_implementation
	{
	private:
		std::unordered_map<std::string, double> _prices;
		std::unordered_map<std::string, ohlcv_data> _ohlcvData;

	public:
		std::string stream_url() const noexcept override { return "wss://stream.bybit.com/spot/quote/ws/v1"; }

		void on_open() override;
		void on_close(std::error_code reason) override;
		void on_fail(std::error_code reason) override;
		void on_message(std::string_view message) override;

		std::string get_order_book_subscription_message(const std::vector<tradable_pair>& tradablePairs) const override;
		std::string get_order_book_unsubscription_message(const std::vector<tradable_pair>& tradablePairs) const override;
		
		std::string get_price_subscription_message(const std::vector<tradable_pair>& tradablePairs) const override;
		std::string get_price_unsubscription_message(const std::vector<tradable_pair>& tradablePairs) const override;
		bool is_price_subscribed(const tradable_pair& pair) const override;
		double get_price(const tradable_pair& pair) const override;

		std::string get_candles_subscription_message(const std::vector<tradable_pair>& tradablePairs, int interval) const override;
		std::string get_candles_unsubscription_message(const std::vector<tradable_pair>& tradablePairs, int interval) const override;
		bool is_candles_subscribed(const tradable_pair& pair, int interval) const override;
		ohlcv_data get_candle(const tradable_pair& pair, int interval) const override;
	};
}