#pragma once

enum class AssetSymbol
{
	GBP,
	USD,
	BTC,
	ETH,
	LTC
};

enum class TradeAction
{
	BUY,
	SELL
};

class TradablePair
{
private:
	const AssetSymbol _asset;
	const AssetSymbol _priceUnit;

public:
	explicit TradablePair(AssetSymbol asset, AssetSymbol priceUnit)
		: _asset{asset}, _priceUnit{priceUnit}
	{}

	AssetSymbol asset() const { return _asset; }
	AssetSymbol price_unit() const { return _priceUnit; }

	bool operator==(const TradablePair& other)
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
			return std::hash<AssetSymbol>()(pair.asset()) ^ std::hash<AssetSymbol>()(pair.price_unit());
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