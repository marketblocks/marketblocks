#include "back_test_market_api.h"
#include "exchanges/exchange_ids.h"

namespace mb
{
	back_test_market_api::back_test_market_api(std::shared_ptr<back_testing_data> backTestingData)
		: _backTestingData{ std::move(backTestingData) }
	{}

	exchange_status back_test_market_api::get_status() const
	{
		return exchange_status::ONLINE;
	}

	std::vector<tradable_pair> back_test_market_api::get_tradable_pairs() const
	{
		return _backTestingData->tradable_pairs();
	}

	std::vector<ohlcv_data> back_test_market_api::get_ohlcv(const tradable_pair& tradablePair, ohlcv_interval interval, int count) const
	{
		return _backTestingData->get_ohlcv(tradablePair, to_seconds(interval), count);
	}

	double back_test_market_api::get_price(const tradable_pair& tradablePair) const
	{
		return _backTestingData->get_price(tradablePair);
	}

	order_book_state back_test_market_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		return _backTestingData->get_order_book(tradablePair, depth);
	}
}