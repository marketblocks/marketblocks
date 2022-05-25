#pragma once

#include <memory>

#include "runner_config.h"
#include "exchanges/exchange.h"
#include "testing/paper_trading/paper_trading_config.h"
#include "testing/back_testing/back_testing_config.h"

namespace mb::internal
{
	std::vector<std::shared_ptr<exchange>> create_exchange_apis(const runner_config& runnerConfig);
}