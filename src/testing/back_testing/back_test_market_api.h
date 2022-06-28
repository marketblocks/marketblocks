#pragma once

#include <vector>

#include "back_testing_data.h"
#include "backtest_websocket_stream.h"
#include "testing/paper_trading/paper_trade_api.h"
#include "exchanges/exchange.h"
#include "exchanges/exchange_status.h"
#include "exchanges/websockets/websocket_stream.h"
#include "trading/tradable_pair.h"
#include "trading/ohlcv_data.h"
#include "trading/order_book.h"

namespace mb
{
	class back_test_market_api : public market_api
	{
	private:
		std::shared_ptr<back_testing_data> _backTestingData;

	public:
		back_test_market_api(std::shared_ptr<back_testing_data> backTestingData);

		exchange_status get_status() const override;
		std::vector<tradable_pair> get_tradable_pairs() const override;
		std::vector<ohlcv_data> get_ohlcv(const tradable_pair& tradablePair, ohlcv_interval interval, int count) const override;
		double get_price(const tradable_pair& tradablePair) const override;
		order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const override;
	};
}