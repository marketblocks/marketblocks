#pragma once

#include <memory>
#include "..\exchange.h"
#include "..\..\networking\httpservice.h"

class KrakenMarketData final : public MarketData
{
private:
	HttpService httpService;

	const std::string build_price_data_uri(const std::vector<TradablePair>& tradablePairs) const;
	const std::vector<TradablePair> read_tradable_pairs(const std::string& jsonResult) const;
	const std::unordered_map<TradablePair, PriceData> read_price_data(const std::string& jsonResult, const std::vector<TradablePair>& tradablePairs) const;

public:
	KrakenMarketData();

	double get_fee() const override;
	const std::vector<TradablePair> get_tradable_pairs() const override;
	const std::unordered_map<TradablePair, PriceData> get_price_data(const std::vector<TradablePair>& tradablePairs) const override;
};

class KrakenTrader final : public Trader
{
	const std::unordered_map<std::string, double> get_all_balances() const override;
	double get_balance(const std::string& tickerId) const override;
	void trade(const TradeDescription& description) override;
};