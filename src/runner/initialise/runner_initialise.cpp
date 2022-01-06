#include <stdexcept>
#include <functional>

#include "runner_initialise.h"
#include "config_file_readers.h"
#include "exchange_factories.h"
#include "exchanges/exchange_id.h"
#include "exchanges/kraken/kraken.h"
#include "exchanges/exchange_assemblers.h"
#include "common/file/file.h"

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

RunnerConfig get_runner_config()
{
	return load_runner_config();

	// if doesn't exist, create
}

TradingOptions get_trading_options() 
{
	return load_trading_options();
}

std::vector<std::shared_ptr<Exchange>> create_exchanges(const RunnerConfig& runnerConfig, RunMode runMode)
{
	std::vector<std::shared_ptr<Exchange>> exchanges;
	ExchangeAssembler assembler = select_assembler(runMode);
	ExchangeIdLookup idLookup;

	for (auto& exchangeId : runnerConfig.exchange_ids())
	{
		exchanges.emplace_back(assembler(create_api_from_id(exchangeId, idLookup)));
	}

	return exchanges;
}