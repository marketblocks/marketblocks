#pragma once

#include <unordered_map>

#include "tradable_pair.h"
#include "trading_constants.h"
#include "order_type.h"
#include "common/utils/financeutils.h"

namespace mb
{
	enum class order_request_parameter
	{
		ASSET_PRICE,
		STOP_PRICE,
		VOLUME,
		TRAILING_DELTA
	};

	class order_request
	{
	private:
		order_type _orderType;
		tradable_pair _pair;
		trade_action _action;
		std::unordered_map<order_request_parameter, double> _parameters;

	public:
		order_request(
			order_type orderType,
			tradable_pair pair,
			trade_action action,
			std::unordered_map<order_request_parameter, double> parameters);

		order_type order_type() const noexcept { return _orderType; }
		const tradable_pair& pair() const noexcept { return _pair; }
		const trade_action& action() const noexcept { return _action; }

		double get(order_request_parameter parameter) const;
	};

	order_request create_limit_order(tradable_pair pair, trade_action action, double assetPrice, double volume);
	order_request create_market_order(tradable_pair pair, trade_action action, double volume);
	order_request create_stop_loss_order(tradable_pair pair, trade_action action, double stopPrice, double volume, double limitPrice = 0);
	order_request create_trailing_stop_loss_order(tradable_pair pair, trade_action action, double delta, double volume, double limitPrice = 0);
}

