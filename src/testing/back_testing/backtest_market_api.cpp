#include "backtest_market_api.h"

namespace mb
{
	exchange_status backtest_market_api::get_status() const
	{
		return exchange_status::ONLINE;
	}

	std::vector<tradable_pair> backtest_market_api::get_tradable_pairs() const
	{
		return _data.tradable_pairs();
	}

	ohlc_data backtest_market_api::get_24h_stats(const tradable_pair& tradablePair) const
	{
		return ohlc_data{ 0,0,0,0,0 };
	}

	std::vector<historical_trade> backtest_market_api::get_historical_trades(const tradable_pair& tradablePair, std::time_t startTime) const
	{
		return {};
	}

	double backtest_market_api::get_price(const tradable_pair& tradablePair) const
	{
		return 0;
	}

	order_book_state backtest_market_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		return order_book_state{ {},{} };
	}
}