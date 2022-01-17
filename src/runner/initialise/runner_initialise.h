#pragma once

#include <vector>
#include <string>

#include "configs.h"
#include "strategy_initialiser.h"
#include "initialisation_error.h"
#include "exchanges/exchange.h"
#include "common/file/config_file_reader.h"
#include "common/types/result.h"

namespace cb::internal
{
	runner_config get_runner_config();
	trading_options get_trading_options();

	std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig, run_mode runMode);
}
