#pragma once

#include <vector>

#include "trading/asset_symbol.h"
#include "common/file/json_wrapper.h"

enum class RunMode
{
	LIVE, LIVETEST, BACKTEST
};

class RunnerConfig
{
private:
	std::vector<std::string> _exchangeIds;

public:
	explicit RunnerConfig(
		std::vector<std::string> exchangeIds);

	const std::vector<std::string>& exchange_ids() const { return _exchangeIds; }

	static RunnerConfig deserialize(JsonWrapper& json);
	std::string serialize() const;
};

class TradingOptions
{
private:
	double _maxTradePercent;
	AssetSymbol _fiatCurrency;

public:
	TradingOptions();
	explicit TradingOptions(double maxTradePercent, AssetSymbol fiatCurrency);

	double max_trade_percent() const { return _maxTradePercent; }
	const AssetSymbol& fiat_currency() const { return _fiatCurrency; }

	static TradingOptions deserialize(JsonWrapper& json);
	std::string serialize() const;
};