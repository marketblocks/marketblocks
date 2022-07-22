#pragma once

#include "tradable_pair.h"
#include "trading_constants.h"
#include "order_type.h"
#include "common/utils/financeutils.h"

namespace mb
{
	class order_request
	{
	private:
		order_type _orderType;
		tradable_pair _pair;
		trade_action _action;
		double _assetPrice;
		double _volume;

	public:
		order_request(
			order_type orderType,
			tradable_pair pair,
			trade_action action,
			double assetPrice,
			double volume);

		order_type order_type() const noexcept { return _orderType; }
		const tradable_pair& pair() const noexcept { return _pair; }
		const trade_action& action() const noexcept { return _action; }
		double asset_price() const noexcept { return _assetPrice; }
		double volume() const noexcept { return _volume; }
	};

	order_request create_order_by_cost(tradable_pair pair, trade_action action, double assetPrice, double tradeCost);
}

