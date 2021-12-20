#pragma once

#include <unordered_map>
#include <string>

#include "common/trading/asset_symbol.h"
#include "common/trading/trading_constants.h"
#include "common/trading/tradable_pair.h"
#include "common/trading/trade_description.h"
#include "common/trading/fee_schedule.h"

class PaperTrader
{
private:
	FeeSchedule _feeSchedule;
	std::unordered_map<AssetSymbol, double> _balances;

	bool has_sufficient_funds(const AssetSymbol& asset, double amount) const;
	TradeResult execute_trade(AssetSymbol gainedAsset, double gainValue, AssetSymbol soldAsset, double soldValue);

public:
	explicit PaperTrader(FeeSchedule feeSchedule, std::unordered_map<AssetSymbol, double> initialBalances);

	const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const;
	const std::unordered_map<AssetSymbol, double> get_balances() const { return _balances; }

	TradeResult trade(const TradeDescription& description);
};