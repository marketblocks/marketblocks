#pragma once

#include <stdexcept>

#include "runner_implementation.h"
#include "default_runner.h"
#include "backtest_runner.h"
#include "runner_config.h"
#include "config_file_reader.h"
#include "logging/logger.h"

namespace mb
{
	void log_version();
	void log_run_mode(run_mode runMode);

	template<typename Strategy>
	class runner
	{
	private:
		std::unique_ptr<internal::runner_implementation<Strategy>> _implementation;
		runner_config _config;
		Strategy _strategy;
		bool _initialised;
		logger& _logger;

	public:
		explicit constexpr runner(
			std::unique_ptr<internal::runner_implementation<Strategy>> implementation,
			runner_config config,
			Strategy strategy)
			: 
			_implementation{ std::move(implementation) }, 
			_config{ std::move(config) },
			_strategy { std::move(strategy) },
			_initialised{ false },
			_logger{ logger::instance() }
		{}

		void initialise()
		{
			_logger.info("Starting initialisation...");

			std::vector<std::shared_ptr<exchange>> exchanges = _implementation->create_exchanges(_config);
			
			_logger.info("Initialising strategy...");
				
			_strategy.initialise(std::move(exchanges));

			_logger.info("Initialisation complete");
			_initialised = true;
		}

		void run()
		{
			if (!_initialised)
			{
				throw mb_exception{ "Runner must be initialized" };
			}

			_implementation->run(_strategy);
		}
	};

	template<typename Strategy, typename... Args>
	runner<Strategy> create_runner(Args&&... args)
	{
		log_version();
		create_config_directory_if_not_exist();

		runner_config config = load_or_create_config<runner_config>();

		log_run_mode(config.runmode());

		std::unique_ptr<internal::runner_implementation<Strategy>> implementation;
		
		if (config.runmode() == run_mode::BACKTEST)
		{
			implementation = std::make_unique<internal::backtest_runner<Strategy>>();
		}
		else
		{
			implementation = std::make_unique<internal::default_runner<Strategy>>();
		}

		return runner<Strategy>
		{
			std::move(implementation),
			std::move(config),
			Strategy{ std::forward<Args>(args)... }
		};
	}
}