#pragma once

#include <vector>

#include "back_testing_data.h"
#include "exchanges/exchange_status.h"
#include "exchanges/websockets/websocket_stream.h"
#include "trading/tradable_pair.h"
#include "trading/ohlc_data.h"
#include "trading/order_book.h"
#include "trading/historical_trade.h"

namespace mb
{
	class null_websocket_stream : public websocket_stream
	{
	public:
		null_websocket_stream()
			: websocket_stream{nullptr, nullptr}
		{}
	};

	class backtest_market_api
	{
	private:
		std::string _id;
		back_testing_data _data;
		null_websocket_stream _websocketStream;

	public:
		backtest_market_api(std::string_view apiName, back_testing_data data)
			: _id{ apiName }, _data{ std::move(data) }, _websocketStream{}
		{ 
			_id += "-back_test";
		}

		constexpr std::string_view id() const noexcept { return _id; }
		websocket_stream& get_websocket_stream() noexcept { return _websocketStream; }

		exchange_status get_status() const;
		std::vector<tradable_pair> get_tradable_pairs() const;
		ohlc_data get_24h_stats(const tradable_pair& tradablePair) const;
		std::vector<historical_trade> get_historical_trades(const tradable_pair& tradablePair, std::time_t startTime) const;
		double get_price(const tradable_pair& tradablePair) const;
		order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const;
	};
}