#pragma once

#include <vector>

#include "back_testing_data.h"
#include "backtest_websocket_stream.h"
#include "exchanges/exchange_status.h"
#include "exchanges/websockets/websocket_stream.h"
#include "trading/tradable_pair.h"
#include "trading/ohlcv_data.h"
#include "trading/order_book.h"

namespace mb
{
	class backtest_market_api
	{
	private:
		std::shared_ptr<back_testing_data_navigator> _dataNavigator;
		std::shared_ptr<backtest_websocket_stream> _websocketStream;

	public:
		backtest_market_api(std::shared_ptr<back_testing_data_navigator> dataNavigator, std::unique_ptr<backtest_websocket_stream> websocketStream);

		const back_testing_data& get_back_testing_data() const noexcept { return _dataNavigator->data(); }
		void increment_data();

		static constexpr std::string_view id() noexcept { return "BACK TEST"; }
		
		std::weak_ptr<websocket_stream> get_websocket_stream() noexcept { return _websocketStream; }

		exchange_status get_status() const;
		std::vector<tradable_pair> get_tradable_pairs() const;
		ohlcv_data get_24h_stats(const tradable_pair& tradablePair) const;
		double get_price(const tradable_pair& tradablePair) const;
		order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const;
	};
}