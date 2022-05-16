#include "backtest_market_api.h"

namespace mb
{
	backtest_market_api::backtest_market_api(std::shared_ptr<back_testing_data_navigator> dataSource, std::unique_ptr<backtest_websocket_stream> websocketStream)
	: _dataNavigator{ std::move(dataSource) }, _websocketStream{ std::move(websocketStream) }
	{
	}

	void backtest_market_api::increment_data()
	{ 
		_dataNavigator->increment_data();

		if (_websocketStream)
		{
			_websocketStream->notify_data_incremented();
		}
	}

	exchange_status backtest_market_api::get_status() const
	{
		return exchange_status::ONLINE;
	}

	std::vector<tradable_pair> backtest_market_api::get_tradable_pairs() const
	{
		return _dataNavigator->data().tradable_pairs();
	}

	ohlcv_data backtest_market_api::get_24h_stats(const tradable_pair& tradablePair) const
	{
		const std::vector<timed_ohlcv_data>& pairData = _dataNavigator->data().get_ohlcv_data(tradablePair);
		std::time_t targetTime = _dataNavigator->data_time() - 86400; // -24hrs
		int startIndex = internal::get_data_index(pairData, _dataNavigator->data_time());

		double open = 0.0;
		double high = 0.0;
		double low = 0.0;
		double close = 0.0;
		double volume = 0.0;

		for (int i = startIndex; i > -1; --i)
		{
			const timed_ohlcv_data& data = pairData.at(i);

			if (data.time_stamp() < targetTime)
			{
				break;
			}

			if (i == startIndex)
			{
				close = data.data().close();
			}

			open = data.data().open();
			high = std::max(high, data.data().high());
			low = std::min(low, data.data().low());
			volume += data.data().volume();
		}

		return ohlcv_data{ open, high, low, close, volume };
	}

	double backtest_market_api::get_price(const tradable_pair& tradablePair) const
	{
		std::optional<std::reference_wrapper<const timed_ohlcv_data>> data{ internal::get_data(*_dataNavigator, tradablePair) };

		if (data.has_value())
		{
			const ohlcv_data& ohlcvData = data.value().get().data();
			return ohlcvData.open();
		}

		return 0.0;
	}

	order_book_state backtest_market_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		return _websocketStream->get_order_book(tradablePair, depth);
	}
}