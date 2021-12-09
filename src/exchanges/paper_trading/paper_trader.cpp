#include "paper_trader.h"
#include "utils/financeutils.h"
#include "utils/containerutils.h"

PaperTrader::PaperTrader(FeeSchedule feeSchedule, std::unordered_map<AssetSymbol, double> initialBalances)
	: _feeSchedule{ std::move(feeSchedule) }, _balances{ std::move(initialBalances) }
{}

bool PaperTrader::has_sufficient_funds(const AssetSymbol& asset, double amount) const
{
	return _balances.at(asset) >= amount;
}

TradeResult PaperTrader::execute_trade(AssetSymbol gainedAsset, double gainValue, AssetSymbol soldAsset, double soldValue)
{
	if (!has_sufficient_funds(soldAsset, soldValue))
	{
		return TradeResult::INSUFFICENT_FUNDS;
	}

	_balances[gainedAsset] += gainValue;
	_balances[soldAsset] -= soldValue;

	return TradeResult::SUCCESS;
}

const std::unordered_map<TradablePair, double> PaperTrader::get_fees(const std::vector<TradablePair>& tradablePairs) const
{
	return to_unordered_map<TradablePair, double>(
		tradablePairs, 
		[](const TradablePair& pair) { return pair; }, 
		[this](const TradablePair& pair) { return _feeSchedule.get_fee(0); });
}

TradeResult PaperTrader::trade(const TradeDescription& description)
{
	double cost = calculate_cost(description.asset_price(), description.volume());
	double fee = cost * _feeSchedule.get_fee(0) * 0.01;

	if (description.action() == TradeAction::BUY)
	{
		return execute_trade(
			description.pair().asset(), 
			description.volume(), 
			description.pair().price_unit(), 
			cost + fee);
	}
	else
	{
		return execute_trade(
			description.pair().price_unit(), 
			cost - fee, 
			description.pair().asset(), 
			description.volume());
	}
}