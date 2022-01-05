#include <stdexcept>
#include <functional>

#include "runner_initialise.h"
#include "exchanges/exchange_constants.h"
#include "exchanges/kraken/kraken.h"
#include "exchanges/exchange_assemblers.h"

namespace
{
	typedef std::shared_ptr<Exchange>(*ExchangeAssembler)(std::unique_ptr<Exchange> api);
	ExchangeAssembler select_assembler(RunMode runMode)
	{
		switch (runMode)
		{
			case RunMode::LIVE:
				return assemble_live;
			case RunMode::LIVETEST:
				return assemble_live_test;
			default:
				throw std::invalid_argument("Invalid run mode");
		}
	}

	std::unique_ptr<Exchange> create_api_from_id(const std::string& identifier, const ExchangeIdLookup& idLookup)
	{
		auto it = idLookup.map().find(identifier);
		if (it == idLookup.map().end())
		{
			throw std::invalid_argument("Exchange identifier: " + identifier + "not recognised");
		}

		ExchangeId id = it->second;

		switch (id)
		{
			case ExchangeId::KRAKEN:
				return make_kraken();
		}
	}
}

RunnerConfig::RunnerConfig(
	RunMode runMode,
	std::vector<std::string> exchangeIds)
	:
	_runMode{ runMode },
	_exchangeIds{ std::move(exchangeIds) }
{}

RunnerConfig load_runner_config()
{
	return RunnerConfig{ RunMode::LIVETEST, std::vector<std::string>{"kraken"}};
}

TradingOptions load_trading_options() 
{
	return TradingOptions { 0.05, AssetSymbol{ "GBP" } };
}

std::vector<std::shared_ptr<Exchange>> create_exchanges(const RunnerConfig& runnerConfig)
{
	std::vector<std::shared_ptr<Exchange>> exchanges;
	ExchangeAssembler assembler = select_assembler(runnerConfig.run_mode());
	ExchangeIdLookup idLookup;

	for (auto& exchangeId : runnerConfig.exchange_ids())
	{
		exchanges.emplace_back(assembler(create_api_from_id(exchangeId, idLookup)));
	}

	return exchanges;
}