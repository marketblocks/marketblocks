#include "trading.h"
#include "utils/financeutils.h"

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