#pragma once

#include "tradable_pair.h"
#include "trading_constants.h"

namespace cb
{
	class trade_description
	{
	private:
		tradable_pair _pair;
		trade_action _action;
		double _assetPrice;
		double _volume;

	public:
		explicit trade_description(tradable_pair pair, trade_action action, double assetPrice, double volume);

		const tradable_pair& pair() const { return _pair; }
		const trade_action& action() const { return _action; }
		double asset_price() const { return _assetPrice; }
		double volume() const { return _volume; }
	};

	trade_description create_trade_by_cost(tradable_pair pair, trade_action action, double assetPrice, double tradeCost);
}

