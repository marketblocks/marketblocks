#include "configs.h"
#include "logging/logger.h"

namespace cb
{
	runner_config::runner_config(
		std::vector<std::string> exchangeIds,
		double tradePercent,
		asset_symbol fiatCurrency)
		:
		_exchangeIds{ std::move(exchangeIds) },
		_tradePercent{ tradePercent },
		_fiatCurrency{ std::move(fiatCurrency) }
	{}

	runner_config runner_config::create_default()
	{
		return runner_config
		{ 
			default_exchange_ids(),
			default_trade_percent(),
			default_fiat_currency()
		};
	}

	runner_config runner_config::deserialize(json_document& json)
	{
		std::vector<std::string> exchangeIds = json.get<std::vector<std::string>>("exchangeIds");
		double maxTradePercent = json.get<double>("maxTradePercent");
		asset_symbol fiatCurrency = asset_symbol{ json.get<std::string>("fiatCurrency") };

		return runner_config
		{ 
			std::move(exchangeIds),
			maxTradePercent,
			std::move(fiatCurrency)
		};
	}

	std::string runner_config::serialize() const
	{
		return "";
	}

	trading_options runner_config::get_trading_options() const
	{
		return trading_options
		{
			_tradePercent,
			_fiatCurrency
		};
	}
}