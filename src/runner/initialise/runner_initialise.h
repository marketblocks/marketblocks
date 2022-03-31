#pragma once

#include <vector>
#include <string>

#include "runner/runner_config.h"
#include "strategy_initialiser.h"
#include "initialisation_error.h"
#include "exchanges/exchange.h"
#include "common/file/config_file_reader.h"
#include "common/types/result.h"
#include "logging/logger.h"

namespace mb::internal
{
	void log_version();

	runner_config get_runner_config();

	std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig);
}
