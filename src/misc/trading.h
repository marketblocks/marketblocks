#pragma once

#include <string>
#include <map>

#include "data.h"

namespace AssetSymbol
{
	constexpr std::string_view GBP{ "GBP" };
	constexpr std::string_view USD{ "USD" };
	constexpr std::string_view BTC{ "BTC" };
	constexpr std::string_view ETH{ "ETH" };
	constexpr std::string_view LTC{ "LTC" };
}

enum class TradeAction
{
	BUY,
	SELL
};

class TradablePair
{
private:
	std::string _exchangeId;
	std::string _asset;
	std::string _priceUnit;

public:
	TradablePair()
		:_exchangeId{}, _asset{}, _priceUnit{}
	{
	}

	TradablePair(std::string asset, std::string priceUnit)
		: _exchangeId{ asset + priceUnit }, _asset {std::move(asset)}, _priceUnit{ std::move(priceUnit)}
	{}

	explicit TradablePair(std::string exchangeId, std::string asset, std::string priceUnit)
		: _exchangeId{std::move(exchangeId)}, _asset{std::move(asset)}, _priceUnit{std::move(priceUnit)}
	{}

	const std::string& exchange_identifier() const { return _exchangeId; }
	const std::string& asset() const { return _asset; }
	const std::string& price_unit() const { return _priceUnit; }
	
	bool contains(const std::string& assetTicker) const { return _asset == assetTicker || _priceUnit == assetTicker; };

	bool operator==(const TradablePair& other) const
	{
		return _asset == other._asset && _priceUnit == other._priceUnit;
	}
};

namespace std
{
	template<>
	struct hash<TradablePair>
	{
		size_t operator()(const TradablePair& pair) const
		{
			return std::hash<std::string_view>()(pair.asset()) ^ std::hash<std::string_view>()(pair.price_unit());
		}
	};
}

class TradeDescription
{
private:
	TradablePair _pair;
	TradeAction _action;
	double _assetPrice;
	double _volume;

public:
	explicit TradeDescription(TradablePair pair, TradeAction action, double assetPrice, double volume)
		: _pair{ std::move(pair) }, _action{ std::move(action) }, _assetPrice{ assetPrice }, _volume{ volume }
	{}

	const TradablePair& pair() const { return _pair; }
	const TradeAction& action() const { return _action; }
	double asset_price() const { return _assetPrice; }
	double volume() const { return _volume; }
};

class TradeResult
{
private:
	bool _success;

public:
	explicit TradeResult(bool success)
		: _success{success}
	{}

	bool success() const { return _success; }
};

class FeeSchedule
{
private:
	std::map<double, double> _fees;

public:
	FeeSchedule(std::map<double, double> fees);

	double get_fee(double tradingVolume) const;
};

class FeeScheduleBuilder
{
private:
	std::map<double, double> fees;

public :
	FeeScheduleBuilder();

	FeeScheduleBuilder add_tier(double tierUpperLimit, double fee);

	FeeSchedule build();
};

TradeDescription create_trade_by_cost(TradablePair pair, TradeAction action, PriceData prices, double tradeCost);
TradeDescription create_trade_by_volume(TradablePair pair, TradeAction action, PriceData prices, double tradeVolume);