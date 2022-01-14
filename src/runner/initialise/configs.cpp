#include "configs.h"

namespace cb
{
	runner_config::runner_config(
		std::vector<std::string> exchangeIds)
		:
		_exchangeIds{ std::move(exchangeIds) }
	{}

	runner_config runner_config::deserialize(json_wrapper& json)
	{
		std::vector<std::string> exchangeIds = json.get_string_array("exchangeIds");
		return runner_config{ std::move(exchangeIds) };
	}

	std::string runner_config::serialize() const
	{
		return "";
	}

	trading_options::trading_options()
		: _maxTradePercent{ 1.0 }, _fiatCurrency{ "GBP" }
	{}

	trading_options::trading_options(double maxTradePercent, asset_symbol fiatCurrency)
		: _maxTradePercent{ maxTradePercent }, _fiatCurrency{ std::move(fiatCurrency) }
	{}

	trading_options trading_options::deserialize(json_wrapper& json)
	{
		double maxTradePercent = json.document()["maxTradePercent"].GetDouble();
		asset_symbol fiatCurrency = asset_symbol{ json.document()["fiatCurrency"].GetString() };

		return trading_options{ maxTradePercent, std::move(fiatCurrency) };
	}

	std::string trading_options::serialize() const
	{
		return "";
	}
}