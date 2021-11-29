#include "paper_trader.h"

PaperTrader::PaperTrader()
	: balances{}
{}

PaperTrader::PaperTrader(std::unordered_map<std::string, double> initialBalances)
	: balances{ initialBalances }
{}

const TradeResult PaperTrader::trade(const TradeDescription& description, double volume, double price)
{
	double cost = volume * price;

	if (description.action() == TradeAction::BUY)
	{
		balances[description.pair().asset()] += volume;
		balances[description.pair().price_unit()] -= cost;
	}
	else
	{
		balances[description.pair().asset()] -= volume;
		balances[description.pair().price_unit()] += cost;
	}

	return TradeResult(true);
}