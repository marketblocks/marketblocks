#include "runner_config.h"
#include "logging/logger.h"
#include "common/exceptions/not_implemented_exception.h"

namespace cb
{
	const std::vector<std::string> runner_config::DEFAULT_EXCHANGE_IDS = std::vector<std::string>{};
	const double runner_config::DEFAULT_TRADE_PERCENT = 0.05;
	const asset_symbol runner_config::DEFAULT_FIAT_CURRENCY = asset_symbol{ "GBP" };

	runner_config::runner_config(
		std::vector<std::string> exchangeIds,
		double tradePercent,
		asset_symbol fiatCurrency)
		:
		_exchangeIds{ std::move(exchangeIds) },
		_tradePercent{ tradePercent },
		_fiatCurrency{ std::move(fiatCurrency) }
	{
		validate();
	}

	runner_config::runner_config()
		: runner_config{ DEFAULT_EXCHANGE_IDS, DEFAULT_TRADE_PERCENT, DEFAULT_FIAT_CURRENCY }
	{}

	void runner_config::validate()
	{
		logger& log{ logger::instance() };

		if (_exchangeIds.empty())
		{
			log.warning("Exchange list is empty, all supported exchanges will be used");
		}

		constexpr double MIN_TRADE_PERCENT = 0.0;
		constexpr double MAX_TRADE_PERCENT = 1.0;

		if (_tradePercent < MIN_TRADE_PERCENT)
		{
			log.warning("Trade Percent value of {0} is less than minimum value. Reset to default value of {}", _tradePercent, DEFAULT_TRADE_PERCENT);
			_tradePercent = DEFAULT_TRADE_PERCENT;
		}
		else if (_tradePercent > MAX_TRADE_PERCENT)
		{
			log.warning("Trade Percent value of {0} is greater than maximum value. Reset to default value of {}", _tradePercent, DEFAULT_TRADE_PERCENT);
			_tradePercent = DEFAULT_TRADE_PERCENT;
		}
	}

	trading_options runner_config::get_trading_options() const
	{
		return trading_options
		{
			_tradePercent,
			_fiatCurrency
		};
	}

	template<>
	runner_config from_json(const json_document& json)
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

	template<>
	json_document to_json(const runner_config& config)
	{
		throw not_implemented_exception{ "runner_config::to_json" };
	}
}