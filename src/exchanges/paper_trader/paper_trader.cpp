#include "paper_trader.h"
#include "utils/financeutils.h"

PaperTrader::PaperTrader()
	: balances{ {"GBP", 1000}}
{}

PaperTrader::PaperTrader(std::unordered_map<std::string, double> initialBalances)
	: balances{ initialBalances }
{}

void PaperTrader::trade(const TradeDescription& description)
{
	double cost = calculate_cost(description.asset_price(), description.volume());

	if (description.action() == TradeAction::BUY)
	{
		balances[description.pair().asset()] += description.volume();
		balances[description.pair().price_unit()] -= cost;
	}
	else
	{
		balances[description.pair().asset()] -= description.volume();
		balances[description.pair().price_unit()] += cost;
	}
}