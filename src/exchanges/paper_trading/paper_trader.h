#pragma once

#include <unordered_map>
#include <string>

#include "exchanges/trader.h"

class PaperTrader final : public Trader
{
private:
	double _fee;
	std::unordered_map<std::string, double> balances;

public:
	explicit PaperTrader(double fee);
	explicit PaperTrader(double fee, std::unordered_map<std::string, double> initialBalances);

	double get_fee(const TradablePair& tradablePair) const;
	const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const override;
	const std::unordered_map<std::string, double> get_all_balances() const override { return balances; }
	double get_balance(const std::string& tickerId) const override { return balances.at(tickerId); }

	void trade(const TradeDescription& description) override;
};