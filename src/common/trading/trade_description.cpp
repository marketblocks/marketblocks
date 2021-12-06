#include "trade_description.h"
#include "utils/financeutils.h"

TradeDescription::TradeDescription(TradablePair pair, TradeAction action, double assetPrice, double volume)
	: _pair{ std::move(pair) }, _action{ std::move(action) }, _assetPrice{ assetPrice }, _volume{ volume }
{}

TradeDescription create_trade_by_cost(TradablePair pair, TradeAction action, PriceData prices, double tradeCost)
{
	double price = select_price(prices, action);
	return TradeDescription{ std::move(pair), action, price, calculate_volume(price, tradeCost) };
}

TradeDescription create_trade_by_volume(TradablePair pair, TradeAction action, PriceData prices, double tradeVolume)
{
	double price = select_price(prices, action);
	return TradeDescription{ std::move(pair), action, price, tradeVolume };
}