#include "trade_description.h"
#include "utils/financeutils.h"

TradeDescription::TradeDescription(TradablePair pair, TradeAction action, double assetPrice, double volume)
	: _pair{ std::move(pair) }, _action{ std::move(action) }, _assetPrice{ assetPrice }, _volume{ volume }
{}

TradeDescription create_trade_by_cost(TradablePair pair, TradeAction action, double assetPrice, double tradeCost)
{
	return TradeDescription{ std::move(pair), action, assetPrice, calculate_volume(assetPrice, tradeCost) };
}