#include "ohlcv_subscription_service.h"
#include "common/utils/containerutils.h"

namespace mb
{
	ohlcv_subscription_service::ohlcv_subscription_service(std::unique_ptr<market_api> marketApi, update_ohlcv_function updateOhlcv, char pairSeparator)
		: 
		_marketApi{ std::move(marketApi) },
		_updateOhlcv{ std::move(updateOhlcv) },
		_pairSeparator{ pairSeparator },
		_subscriptions{}
	{}

	bool ohlcv_subscription_service::is_subscribed(std::string_view pairName) const
	{
		return contains(_subscriptions, pairName.data());
	}

	void ohlcv_subscription_service::add_subscription(const websocket_subscription& subscription)
	{
		ohlcv_interval ohlcvInterval{ subscription.get_ohlcv_interval() };
		int interval{ to_seconds(ohlcvInterval) };

		for (auto& pair : subscription.pair_item())
		{
			std::vector<ohlcv_data> ohlcvData{ _marketApi->get_ohlcv(pair, ohlcvInterval, 1) };
			ohlcv_data latestOhlcv{ ohlcvData.empty() ? ohlcv_data{} : ohlcvData.front() };

			std::string pairName{ pair.to_string(_pairSeparator) };

			_subscriptions[pairName].emplace(ohlcvInterval, ohlcv_from_trades{ latestOhlcv, interval });
			_updateOhlcv(std::move(pairName), ohlcvInterval, std::move(latestOhlcv));
		}
	}

	void ohlcv_subscription_service::remove_subscription(const websocket_subscription& subscription)
	{
		ohlcv_interval interval{ subscription.get_ohlcv_interval() };

		for (auto& pair : subscription.pair_item())
		{
			std::string pairName{ pair.to_string(_pairSeparator) };
			
			_subscriptions[pairName].erase(interval);
			
			if (_subscriptions[pairName].empty())
			{
				_subscriptions.erase(pairName);
			}
		}
	}

	void ohlcv_subscription_service::update_ohlcv(std::string_view pairName, std::time_t time, double price, double volume)
	{
		auto it = _subscriptions.find(pairName.data());

		if (it == _subscriptions.end())
		{
			throw mb_exception{ fmt::format("OHLCV is not subscribed for pair {}", pairName) };
		}

		std::unordered_map<ohlcv_interval, ohlcv_from_trades>& ohlcvFromTrades{ it->second };

		for (auto& [interval, oft] : ohlcvFromTrades)
		{
			oft.add_trade(time, price, volume);
			_updateOhlcv(std::string{ pairName }, interval, oft.get_ohlcv(time));
		}
	}
}