#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "exchanges/market_data.h"
#include "exchanges/trader.h"
#include "networking/http/http_service.h"

class KrakenApi final : public MarketData, public Trader
{
private:
	std::vector<unsigned char> decodedSecret;
	HttpService _httpService;
	std::string get_nonce() const;
	std::string compute_api_sign(const std::string& uriPath, const std::string& postData, const std::string& nonce) const;

public:
	KrakenApi(HttpService httpService);

	// Market Data
	const std::vector<TradablePair> get_tradable_pairs() const override;
	const std::unordered_map<TradablePair, OrderBookState> get_order_book(const std::vector<TradablePair>& tradablePairs, int depth) const override;

	// Trader
	const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const override;
	const std::unordered_map<AssetSymbol, double> get_balances() const override;

	TradeResult trade(const TradeDescription& description) override;
};