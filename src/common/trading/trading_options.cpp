#include "trading_options.h"

TradingOptions::TradingOptions(double maxTradePercent, AssetSymbol fiatCurrency)
	: _maxTradePercent{ maxTradePercent }, _fiatCurrency{ std::move(fiatCurrency) }
{
}