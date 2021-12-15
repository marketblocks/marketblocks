#pragma once

#include <unordered_map>
#include <string>

#include "exchanges/trader.h"
#include "common/trading/fee_schedule.h"

class PaperTrader final : public Trader
{
private:
	FeeSchedule _feeSchedule;
	std::unordered_map<AssetSymbol, double> _balances;

	bool has_sufficient_funds(const AssetSymbol& asset, double amount) const;
	TradeResult execute_trade(AssetSymbol gainedAsset, double gainValue, AssetSymbol soldAsset, double soldValue);

public:
	explicit PaperTrader(FeeSchedule feeSchedule, std::unordered_map<AssetSymbol, double> initialBalances);

	const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const override;
	const std::unordered_map<AssetSymbol, double> get_balances() const override { return _balances; }

	TradeResult trade(const TradeDescription& description) override;
};