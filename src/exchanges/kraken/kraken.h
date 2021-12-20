#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "exchanges/exchange.h"
#include "networking/http/http_service.h"
#include "common/trading/trade_description.h"
#include "common/trading/trading_constants.h"
#include "common/trading/order_book.h"

class KrakenApi final : public Exchange
{
private:
	std::vector<unsigned char> decodedSecret;
	HttpService _httpService;

	std::string get_nonce() const;
	std::string compute_api_sign(const std::string& uriPath, const std::string& postData, const std::string& nonce) const;

	std::string send_public_request(const std::string& method, const std::string& query) const;
	std::string send_public_request(const std::string& method) const;

	std::string send_private_request(const std::string& method, const std::string& query) const;
	std::string send_private_request(const std::string& method) const;

public:
	KrakenApi(HttpService httpService);

	const std::vector<TradablePair> get_tradable_pairs() const override;
	const std::unordered_map<TradablePair, OrderBookState> get_order_book(const std::vector<TradablePair>& tradablePairs, int depth) const override;
	const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const override;
	const std::unordered_map<AssetSymbol, double> get_balances() const override;
	TradeResult trade(const TradeDescription& description) override;
};