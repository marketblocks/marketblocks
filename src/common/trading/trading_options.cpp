#include "trading_options.h"

TradingOptions::TradingOptions()
	: _maxTradePercent{ 1.0 }, _fiatCurrency{ "GBP" }
{}

TradingOptions::TradingOptions(double maxTradePercent, AssetSymbol fiatCurrency)
	: _maxTradePercent{ maxTradePercent }, _fiatCurrency{ std::move(fiatCurrency) }
{}