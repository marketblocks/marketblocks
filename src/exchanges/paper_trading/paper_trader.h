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

public:
	explicit PaperTrader(FeeSchedule feeSchedule, std::unordered_map<AssetSymbol, double> initialBalances);

	double get_fee(const TradablePair& tradablePair) const;
	const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const override;
	const std::unordered_map<AssetSymbol, double> get_balances() const override { return _balances; }

	void trade(const TradeDescription& description) override;
};