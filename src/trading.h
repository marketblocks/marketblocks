#pragma once

enum class AssetSymbol
{
	GBP,
	BTC
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
};

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