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

	ohlcv_data backtest_market_api::get_24h_stats(const tradable_pair& tradablePair) const
	{
		return ohlcv_data{ 0,0,0,0,0 };
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