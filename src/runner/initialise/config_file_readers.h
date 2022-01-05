#pragma once

#include <string>

#include "configs.h"
#include "exchanges/kraken/kraken_config.h"

RunnerConfig load_runner_config();
void save_runner_config(const RunnerConfig& config);

TradingOptions load_trading_options();
void save_trading_options(const TradingOptions& options);

KrakenConfig load_kraken_config();
void save_kraken_config(const KrakenConfig& config);