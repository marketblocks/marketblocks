#pragma once

#include <string>

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
	const std::string _exchangeId;
	const std::string _asset;
	const std::string _priceUnit;

public:
	TradablePair()
		: _exchangeId{}, _asset{}, _priceUnit{}
	{}

	explicit TradablePair(std::string asset, std::string priceUnit)
		: _exchangeId{}, _asset{ std::move(asset) }, _priceUnit{ std::move(priceUnit) }
	{}

	explicit TradablePair(std::string exchangeId, std::string asset, std::string priceUnit)
		: _exchangeId{ std::move(exchangeId) }, _asset{std::move(asset)}, _priceUnit{std::move(priceUnit)}
	{}

	const std::string& exchange_identifier() const { return _exchangeId; }
	const std::string& asset() const { return _asset; }
	const std::string& price_unit() const { return _priceUnit; }
	
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
	const TradablePair _pair;
	const TradeAction _action;

public:
	explicit TradeDescription(TradablePair pair, TradeAction action)
		: _pair{pair}, _action{action}
	{}

	const TradablePair pair() const { return _pair; }
	const TradeAction action() const { return _action; }
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