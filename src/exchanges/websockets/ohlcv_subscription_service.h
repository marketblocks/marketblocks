#pragma once

#include <string_view>
#include <functional>

#include "exchanges/exchange.h"
#include "trading/ohlcv_from_trades.h"

namespace mb
{
	class ohlcv_subscription_service
	{
	private:
		using update_ohlcv_function = std::function<void(std::string, ohlcv_interval, ohlcv_data)>;

		std::unordered_map<std::string,std::unordered_map<ohlcv_interval, ohlcv_from_trades>> _subscriptions;
		std::unique_ptr<market_api> _marketApi;
		update_ohlcv_function _updateOhlcv;
		char _pairSeparator;

	public:
		ohlcv_subscription_service(std::unique_ptr<market_api> marketApi, update_ohlcv_function updateOhlcv, char pairSeparator);

		bool is_subscribed(std::string_view pairName) const;
		void add_subscription(const websocket_subscription& subscription);
		void remove_subscription(const websocket_subscription& subscription);
		void update_ohlcv(std::string_view pairName, std::time_t time, double price, double volume);
	};
}