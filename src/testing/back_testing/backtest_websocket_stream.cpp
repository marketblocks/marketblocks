#include "backtest_websocket_stream.h"
#include "trading/ohlcv_data.h"

#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	backtest_websocket_stream::backtest_websocket_stream(std::shared_ptr<back_testing_data> backTestingData)
		: _backTestingData{ std::move(backTestingData) }
	{}

	void backtest_websocket_stream::notify()
	{
		for (auto& subscription : _subscriptions)
		{
			switch (subscription.channel())
			{
			case websocket_channel::TRADE:
			{
				if (has_trade_update_handler())
				{
					fire_trade_update(trade_update_message
						{ 
							subscription.pair_item(),
							_backTestingData->get_trade(subscription.pair_item())
						});
				}

				break;
			}
			case websocket_channel::OHLCV:
			{
				if (has_ohlcv_update_handler())
				{
					std::vector<ohlcv_data> ohlcvData{ _backTestingData->get_ohlcv(subscription.pair_item(), to_seconds(subscription.get_ohlcv_interval()), 1) };
					ohlcv_data messageData = ohlcvData.empty()
						? ohlcv_data{}
						: ohlcvData.front();

					fire_ohlcv_update(ohlcv_update_message
						{ 
							subscription.pair_item(), 
							subscription.get_ohlcv_interval(), 
							std::move(messageData)
						});
				}

				break;
			}
			case websocket_channel::ORDER_BOOK:
			{
				if (has_order_book_update_handler())
				{
					fire_order_book_update(order_book_update_message
						{ 
							subscription.pair_item(), 
							_backTestingData->get_order_book(subscription.pair_item()).asks().front()
						});
				}

				break;
			}
			}
		}
	}

	void backtest_websocket_stream::subscribe(const websocket_subscription& subscription)
	{
		for (auto& pair : subscription.pair_item())
		{
			_subscriptions.emplace(unique_websocket_subscription
				{
					subscription.channel(),
					pair,
					subscription.get_parameter()
				});
		}
	}

	void backtest_websocket_stream::unsubscribe(const websocket_subscription& subscription)
	{
		for (auto& pair : subscription.pair_item())
		{
			_subscriptions.erase(unique_websocket_subscription
				{
					subscription.channel(),
					pair,
					subscription.get_parameter()
				});
		}
	}

	subscription_status backtest_websocket_stream::get_subscription_status(const unique_websocket_subscription& subscription) const
	{
		return _subscriptions.find(subscription) == _subscriptions.end()
			? subscription_status::UNSUBSCRIBED
			: subscription_status::SUBSCRIBED;
	}

	order_book_state backtest_websocket_stream::get_order_book(const tradable_pair& pair, int depth) const
	{
		return _backTestingData->get_order_book(pair, depth);
	}

	trade_update backtest_websocket_stream::get_last_trade(const tradable_pair& pair) const
	{
		return _backTestingData->get_trade(pair);
	}

	ohlcv_data backtest_websocket_stream::get_last_candle(const tradable_pair& pair, ohlcv_interval interval) const
	{
		std::vector<ohlcv_data> candles = _backTestingData->get_ohlcv(pair, to_seconds(interval), 1);

		if (candles.empty())
		{
			return ohlcv_data{};
		}

		return candles.front();
	}
}