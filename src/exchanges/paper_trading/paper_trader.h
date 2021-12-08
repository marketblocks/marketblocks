#pragma once

#include <unordered_map>
#include <string>

#include "exchanges/trader.h"
#include "common/trading/fee_schedule.h"

class PaperTrader final : public Trader
{
private:
	FeeSchedule _feeSchedule;
	std::unordered_map<std::string, double> _balances;

public:
	explicit PaperTrader();
	explicit PaperTrader(FeeSchedule feeSchedule, std::unordered_map<std::string, double> initialBalances);

	double get_fee(const TradablePair& tradablePair) const;
	const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const override;
	const std::unordered_map<std::string, double> get_all_balances() const override { return _balances; }
	double get_balance(const std::string& tickerId) const override { return _balances.at(tickerId); }

	void trade(const TradeDescription& description) override;
};