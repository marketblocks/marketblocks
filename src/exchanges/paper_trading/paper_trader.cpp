#include "paper_trader.h"
#include "utils/financeutils.h"
#include "utils/vectorutils.h"

PaperTrader::PaperTrader(double fee)
	: _fee{ fee }, balances{ {"GBP", 1000} }
{}

PaperTrader::PaperTrader(double fee, std::unordered_map<std::string, double> initialBalances)
	: _fee{ fee }, balances{ initialBalances }
{}

double PaperTrader::get_fee(const TradablePair& tradablePair) const
{
	return _fee;
}

const std::unordered_map<TradablePair, double> PaperTrader::get_fees(const std::vector<TradablePair>& tradablePairs) const
{
	return to_unordered_map<TradablePair, double>(
		tradablePairs, 
		[](const TradablePair& pair) { return pair; }, 
		[this](const TradablePair& pair) { return _fee; });
}

void PaperTrader::trade(const TradeDescription& description)
{
	double cost = calculate_cost(description.asset_price(), description.volume());
	double fee = cost * get_fee(description.pair()) * 0.01;

	if (description.action() == TradeAction::BUY)
	{
		balances[description.pair().asset()] += description.volume();
		balances[description.pair().price_unit()] -= cost + fee;
	}
	else
	{
		balances[description.pair().asset()] -= description.volume();
		balances[description.pair().price_unit()] += cost - fee;
	}
}