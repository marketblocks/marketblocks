#pragma once

#include "tradable_pair.h"
#include "trading_constants.h"
#include "order_type.h"
#include "common/utils/financeutils.h"

namespace cb
{
	class trade_description
	{
	private:
		order_type _orderType;
		tradable_pair _pair;
		trade_action _action;
		double _assetPrice;
		double _volume;

	public:
		constexpr trade_description(
			order_type orderType,
			tradable_pair pair,
			trade_action action,
			double assetPrice,
			double volume)
			:
			_orderType{ orderType },
			_pair{ std::move(pair) },
			_action{ std::move(action) },
			_assetPrice{ assetPrice },
			_volume{ volume }
		{}

		constexpr order_type order_type() const noexcept { return _orderType; }
		constexpr const tradable_pair& pair() const noexcept { return _pair; }
		constexpr const trade_action& action() const noexcept { return _action; }
		constexpr double asset_price() const noexcept { return _assetPrice; }
		constexpr double volume() const noexcept { return _volume; }
	};

	constexpr trade_description create_trade_by_cost(tradable_pair pair, trade_action action, double assetPrice, double tradeCost)
	{
		return trade_description{ order_type::LIMIT, std::move(pair), action, assetPrice, calculate_volume(assetPrice, tradeCost) };
	}
}

