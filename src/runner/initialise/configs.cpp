#include "configs.h"

RunnerConfig::RunnerConfig(
	std::vector<std::string> exchangeIds)
	:
	_exchangeIds{ std::move(exchangeIds) }
{}

RunnerConfig RunnerConfig::deserialize(JsonWrapper& json)
{
	std::vector<std::string> exchangeIds = json.get_string_array("exchangeIds");
	return RunnerConfig{ std::move(exchangeIds) };
}

std::string RunnerConfig::serialize() const
{
	return "";
}

TradingOptions::TradingOptions()
	: _maxTradePercent{ 1.0 }, _fiatCurrency{ "GBP" }
{}

TradingOptions::TradingOptions(double maxTradePercent, AssetSymbol fiatCurrency)
	: _maxTradePercent{ maxTradePercent }, _fiatCurrency{ std::move(fiatCurrency) }
{}

TradingOptions TradingOptions::deserialize(JsonWrapper& json)
{
	double maxTradePercent = json.document()["maxTradePercent"].GetDouble();
	AssetSymbol fiatCurrency = AssetSymbol{ json.document()["fiatCurrency"].GetString() };

	return TradingOptions{ maxTradePercent, std::move(fiatCurrency) };
}

std::string TradingOptions::serialize() const
{
	return "";
}