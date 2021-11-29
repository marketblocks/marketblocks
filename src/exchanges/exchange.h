#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "..\misc\data.h"
#include "..\misc\trading.h"

#include <iostream>

class MarketData
{
public:
	virtual ~MarketData() = default;

	virtual double get_fee() const = 0;
	virtual const std::vector<TradablePair> get_tradable_pairs() const = 0;
	virtual const std::unordered_map<TradablePair, PriceData> get_price_data(const std::vector<TradablePair>& tradablePairs) const = 0;
};

class Trader
{
public:
	virtual ~Trader() = default;

	virtual const std::unordered_map<std::string, double> get_all_balances() const = 0;
	virtual double get_balance(const std::string& tickerId) const = 0;

	virtual const TradeResult trade(const TradeDescription& description, double volume, double price) = 0;
};

class Exchange final
{
private:
	std::unique_ptr<MarketData> _marketData;
	std::unique_ptr<Trader> _trader;

public:
	Exchange(std::unique_ptr<MarketData> marketData, std::unique_ptr<Trader> trader)
		: _marketData{std::move(marketData)}, _trader{std::move(trader)}
	{}

	double get_fee() const 
	{ 
		return _marketData->get_fee(); 
	}

	const std::vector<TradablePair> get_tradable_pairs() const 
	{
		return _marketData->get_tradable_pairs();
	}

	const std::unordered_map<TradablePair, PriceData> get_price_data(const std::vector<TradablePair>& tradablePairs) const 
	{ 
		return _marketData->get_price_data(tradablePairs);
	}

	const std::unordered_map<std::string, double> get_account_balance() const
	{
		return _trader->get_all_balances();
	}

	double get_balance(const std::string& tickerId) const
	{
		return _trader->get_balance(tickerId);
	}

	const TradeResult trade(const TradeDescription& description, double volume, double price)
	{
		return _trader->trade(description, volume, price);
	}
};