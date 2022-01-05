#pragma once

#include <vector>
#include <string>

#include "configs.h"
#include "strategy_initialiser.h"
#include "exchanges/exchange.h"

RunnerConfig get_runner_config();
TradingOptions get_trading_options();
std::vector<std::shared_ptr<Exchange>> create_exchanges(const RunnerConfig& runnerConfig, RunMode runMode);