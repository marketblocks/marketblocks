#include "paper_trader.h"
#include "utils/financeutils.h"
#include "utils/containerutils.h"

PaperTrader::PaperTrader(FeeSchedule feeSchedule, std::unordered_map<AssetSymbol, double> initialBalances)
	: _feeSchedule{ std::move(feeSchedule) }, _balances{ std::move(initialBalances) }
{}

double PaperTrader::get_fee(const TradablePair& tradablePair) const
{
	return _feeSchedule.get_fee(0);
}

const std::unordered_map<TradablePair, double> PaperTrader::get_fees(const std::vector<TradablePair>& tradablePairs) const
{
	return to_unordered_map<TradablePair, double>(
		tradablePairs, 
		[](const TradablePair& pair) { return pair; }, 
		[this](const TradablePair& pair) { return get_fee(pair); });
}

void PaperTrader::trade(const TradeDescription& description)
{
	double cost = calculate_cost(description.asset_price(), description.volume());
	double fee = cost * get_fee(description.pair()) * 0.01;

	if (description.action() == TradeAction::BUY)
	{
		_balances[description.pair().asset()] += description.volume();
		_balances[description.pair().price_unit()] -= cost + fee;
	}
	else
	{
		_balances[description.pair().asset()] -= description.volume();
		_balances[description.pair().price_unit()] += cost - fee;
	}
}