#pragma once

#include <vector>
#include <string>

#include "configs.h"
#include "strategy_initialiser.h"
#include "initialisation_error.h"
#include "exchanges/exchange.h"
#include "common/file/config_file_reader.h"
#include "common/types/result.h"
#include "logging/logger.h"

namespace cb::internal
{
	template<typename Config>
	Config get_config()
	{
		try
		{
			std::string configName{ Config::name() };
			logger& log{ logger::instance() };

			log.info("Reading config file: {}", configName);

			Config config = load_or_create_config<Config>();

			log.info("{} read successfully", configName);

			return config;
		}
		catch (const cb_exception& e)
		{
			throw initialisation_error{ std::format("Error occured reading {0}: {1}", Config::name(), e.what())};
		}
	}

	runner_config get_runner_config();
	trading_options get_trading_options();

	std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig, run_mode runMode);
}
