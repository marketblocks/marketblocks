#pragma once

#include <vector>
#include <string>

#include "strategy_initialiser.h"
#include "exchanges/exchange.h"
#include "common/trading/trading_options.h"

enum class RunMode
{
	LIVE, LIVETEST, BACKTEST
};

class RunnerConfig
{
private:
	RunMode _runMode;
	std::vector<std::string> _exchangeIds;

public:
	explicit RunnerConfig(
		RunMode runMode,
		std::vector<std::string> exchangeIds);

	RunMode run_mode() const { return _runMode; }
	const std::vector<std::string>& exchange_ids() const { return _exchangeIds; }
};

RunnerConfig load_runner_config();
TradingOptions load_trading_options();
std::vector<std::shared_ptr<Exchange>> create_exchanges(const RunnerConfig& runnerConfig);