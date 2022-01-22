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

namespace cb::internal
{
	template<typename Config>
	Config get_config()
	{
		logger& log{ logger::instance() };
		std::string configName{ Config::name() };

		log.info("Reading config file: {}", configName);

		try
		{
			Config config = load_or_create_config<Config>();

			log.info("{} read successfully", configName);
			return config;
		}
		catch (const std::exception& e)
		{
			log.error("Error occurred reading {0}: {1}, using default values", configName, e.what());
			return Config{};
		}
	}

	runner_config get_runner_config();

	std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig, run_mode runMode);
}
