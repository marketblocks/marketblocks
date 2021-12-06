#include "trading_options.h"

TradingOptions::TradingOptions(double maxTradePercent, std::string fiatCurrency)
	: _maxTradePercent{ maxTradePercent }, _fiatCurrency{ std::move(fiatCurrency) }
{
}