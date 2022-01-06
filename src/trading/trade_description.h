#pragma once

#include "tradable_pair.h"
#include "trading_constants.h"

class TradeDescription
{
private:
	TradablePair _pair;
	TradeAction _action;
	double _assetPrice;
	double _volume;

public:
	explicit TradeDescription(TradablePair pair, TradeAction action, double assetPrice, double volume);

	const TradablePair& pair() const { return _pair; }
	const TradeAction& action() const { return _action; }
	double asset_price() const { return _assetPrice; }
	double volume() const { return _volume; }
};

TradeDescription create_trade_by_cost(TradablePair pair, TradeAction action, double assetPrice, double tradeCost);
