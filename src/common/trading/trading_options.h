#pragma once

#include <string>

#include "asset_symbol.h"

class TradingOptions
{
private:
	double _maxTradePercent;
	AssetSymbol _fiatCurrency;

public:
	explicit TradingOptions(double maxTradePercent, AssetSymbol fiatCurrency);

	double max_trade_percent() const { return _maxTradePercent; }
	const AssetSymbol& fiat_currency() const { return _fiatCurrency; }
};