#pragma once

#include "exchanges/websockets/websocket_stream_implementation.h"

#include "common/exceptions/not_implemented_exception.h"

namespace mb::internal
{
	class coinbase_websocket_stream : public websocket_stream_implementation
	{
	public:
		std::string stream_url() const noexcept override { return "wss://ws-feed.exchange.coinbase.com"; }

		void on_open() override;
		void on_close(std::error_code reason) override;
		void on_fail(std::error_code reason) override;
		void on_message(std::string_view message) override;

		std::string get_order_book_subscription_message(const std::vector<tradable_pair>& tradablePairs) const override;
		std::string get_order_book_unsubscription_message(const std::vector<tradable_pair>& tradablePairs) const override;

		std::string get_price_subscription_message(const std::vector<tradable_pair>& tradablePairs) const override
		{
			throw not_implemented_exception{ "coinbase_websocket::get_price" };
		}

		std::string get_price_unsubscription_message(const std::vector<tradable_pair>& tradablePairs) const override
		{
			throw not_implemented_exception{ "coinbase_websocket::get_price" };
		}

		bool is_price_subscribed(const tradable_pair& pair) const override
		{
			throw not_implemented_exception{ "coinbase_websocket::get_price" };
		}

		double get_price(const tradable_pair& pair) const override
		{
			throw not_implemented_exception{ "coinbase_websocket::get_price" };
		}

		std::string get_candles_subscription_message(const std::vector<tradable_pair>& tradablePairs, int interval) const override
		{
			throw not_implemented_exception{ "coinbase_websocket::get_candles" };
		}

		std::string get_candles_unsubscription_message(const std::vector<tradable_pair>& tradablePairs, int interval) const override
		{
			throw not_implemented_exception{ "coinbase_websocket::get_candles" };
		}

		bool is_candles_subscribed(const tradable_pair& pair, int interval) const override
		{
			throw not_implemented_exception{ "coinbase_websocket::get_candles" };
		}

		ohlcv_data get_candle(const tradable_pair& pair, int interval) const override
		{
			throw not_implemented_exception{ "coinbase_websocket::get_candles" };
		}
	};
}