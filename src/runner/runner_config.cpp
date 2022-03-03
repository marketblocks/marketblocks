#include "runner_config.h"
#include "logging/logger.h"
#include "common/exceptions/not_implemented_exception.h"

namespace
{
	namespace json_property_names
	{
		const std::string exchange_ids() { return "exchangeIds"; }
		const std::string trade_percent() { return "tradePercent"; }
		const std::string fiat_currency() { return "fiatCurrency"; }
	}
}

namespace cb
{
	runner_config::runner_config()
		: runner_config{ {}, DEFAULT_TRADE_PERCENT, asset_symbol{ "GBP" } }
	{}

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

	template<>
	runner_config from_json(const json_document& json)
	{
		std::vector<std::string> exchangeIds = json.get<std::vector<std::string>>(json_property_names::exchange_ids());
		double maxTradePercent = json.get<double>(json_property_names::trade_percent());
		asset_symbol fiatCurrency = asset_symbol{ json.get<std::string>(json_property_names::fiat_currency()) };

		return runner_config
		{
			std::move(exchangeIds),
			maxTradePercent,
			std::move(fiatCurrency)
		};
	}

	template<>
	void to_json(const runner_config& config, json_writer& writer)
	{
		writer.add(json_property_names::exchange_ids(), config.exchange_ids());
		writer.add(json_property_names::trade_percent(), config.trade_percent());
		writer.add(json_property_names::fiat_currency(), config.fiat_currency().get());
	}
}