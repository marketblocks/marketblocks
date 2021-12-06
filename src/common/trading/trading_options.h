#pragma once

#include <string>

class TradingOptions
{
private:
	double _maxTradePercent;
	std::string _fiatCurrency;

public:
	explicit TradingOptions(double maxTradePercent, std::string fiatCurrency);

	double max_trade_percent() const { return _maxTradePercent; }
	const std::string& fiat_currency() const { return _fiatCurrency; }
};