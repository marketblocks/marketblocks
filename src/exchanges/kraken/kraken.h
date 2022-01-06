#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "kraken_config.h"
#include "exchanges/exchange.h"
#include "networking/http/http_service.h"
#include "trading/trade_description.h"
#include "trading/trading_constants.h"
#include "trading/order_book.h"

struct KrakenConstants
{
	inline static const std::string BASEURL = "https://api.kraken.com";
	inline static const std::string VERSION = "0";
	inline static const std::string PUBLIC = "public";
	inline static const std::string PRIVATE = "private";
	
	inline static const std::string TRADABLE_PAIRS = "AssetPairs";
	inline static const std::string ORDER_BOOK = "Depth";
	inline static const std::string BALANCE = "Balance";
};

class KrakenApi final : public Exchange
{
private:
	KrakenConstants _constants;

	std::string _publicKey;
	std::vector<unsigned char> _decodedPrivateKey;
	HttpService _httpService;

	std::string build_url_path(const std::string& access, const std::string& method) const;
	std::string build_kraken_url(const std::string& access, const std::string& method, const std::string& query) const;

	std::string get_nonce() const;
	std::string compute_api_sign(const std::string& uriPath, const std::string& postData, const std::string& nonce) const;

	std::string send_public_request(const std::string& method, const std::string& query) const;
	std::string send_public_request(const std::string& method) const;

	std::string send_private_request(const std::string& method, const std::string& query) const;
	std::string send_private_request(const std::string& method) const;

public:
	KrakenApi(KrakenConfig config, HttpService httpService);

	const std::vector<TradablePair> get_tradable_pairs() const override;
	const std::unordered_map<TradablePair, OrderBookState> get_order_book(const std::vector<TradablePair>& tradablePairs, int depth) const override;
	const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const override;
	const std::unordered_map<AssetSymbol, double> get_balances() const override;
	TradeResult trade(const TradeDescription& description) override;
};