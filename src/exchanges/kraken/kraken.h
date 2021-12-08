#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "exchanges/market_data.h"
#include "exchanges/trader.h"
#include "networking/httpservice.h"

class KrakenApi final : public MarketData, Trader
{
private:
	HttpService _httpService;

public:
	KrakenApi(HttpService httpService);

	const std::vector<TradablePair> get_tradable_pairs() const override;
	const std::unordered_map<TradablePair, PriceData> get_price_data(const std::vector<TradablePair>& tradablePairs) const override;

	double get_fee(const TradablePair& tradablePair) const override;
	const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const override;
	double get_balance(const std::string& tickerId) const override;
	const std::unordered_map<std::string, double> get_all_balances() const override;
	void trade(const TradeDescription& description) override;
};