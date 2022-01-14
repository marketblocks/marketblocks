#pragma once

#include <string>

#include "configs.h"
#include "exchanges/kraken/kraken_config.h"

namespace cb
{
	runner_config load_runner_config();
	void save_runner_config(const runner_config& config);

	trading_options load_trading_options();
	void save_trading_options(const trading_options& options);

	kraken_config load_kraken_config();
	void save_kraken_config(const kraken_config& config);
}