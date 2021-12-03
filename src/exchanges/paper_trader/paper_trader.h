#pragma once

#include "exchanges/exchange.h"

class PaperTrader final : public Trader
{
private:
	std::unordered_map<std::string, double> balances;

public:
	PaperTrader();
	explicit PaperTrader(std::unordered_map<std::string, double> initialBalances);

	const std::unordered_map<std::string, double> get_all_balances() const override { return balances; }
	double get_balance(const std::string& tickerId) const override { return balances.at(tickerId); }

	void trade(const TradeDescription& description) override;
};