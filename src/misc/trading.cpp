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

FeeSchedule::FeeSchedule(std::map<double, double> fees)
	: _fees{ std::move(fees) }
{}

double FeeSchedule::get_fee(double tradingVolume) const
{
	for (auto& [tierLimit, fee] : _fees)
	{
		if (tradingVolume < tierLimit)
		{
			return fee;
		}
	}

	return 0.0;
}

FeeScheduleBuilder::FeeScheduleBuilder()
	: fees{}
{}

FeeScheduleBuilder FeeScheduleBuilder::add_tier(double tierUpperLimit, double fee)
{
	fees.emplace(tierUpperLimit, fee);

	return *this;
}

FeeSchedule FeeScheduleBuilder::build()
{
	return FeeSchedule{ fees };
}