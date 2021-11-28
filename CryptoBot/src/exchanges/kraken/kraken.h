#pragma once

#include <memory>
#include "..\exchange.h"
#include "..\..\networking\httpservice.h"

class KrakenExchange final : public Exchange
{
private:
	HttpService httpService;

	const std::string build_price_data_uri(const std::vector<TradablePair>& tradablePairs) const;
	const std::vector<TradablePair> read_tradable_pairs(const std::string& jsonResult) const;
	const std::unordered_map<TradablePair, PriceData> read_price_data(const std::string& jsonResult, const std::vector<TradablePair>& tradablePairs) const;

public:
	KrakenExchange();

	virtual double get_fee() const override;
	virtual const std::vector<TradablePair> get_tradable_pairs() const override;
	virtual const std::unordered_map<TradablePair, PriceData> get_price_data(const std::vector<TradablePair>& tradablePairs) const override;
};