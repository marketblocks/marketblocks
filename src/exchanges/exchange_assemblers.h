#pragma once

#include <memory>

#include "exchange.h"
#include "exchanges/paper_trading/paper_trader.h"

namespace mb
{
	std::shared_ptr<exchange> assemble_live(std::unique_ptr<exchange> api);
	std::shared_ptr<exchange> assemble_live_test(std::unique_ptr<exchange> api, paper_trading_config paperTradingConfig);
}