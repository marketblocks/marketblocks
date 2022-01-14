#pragma once

#include <vector>
#include <string>

#include "configs.h"
#include "strategy_initialiser.h"
#include "exchanges/exchange.h"

namespace cb::internal
{
	runner_config get_runner_config();
	trading_options get_trading_options();
	std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig, run_mode runMode);
}
