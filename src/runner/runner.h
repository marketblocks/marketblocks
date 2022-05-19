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
		std::unique_ptr<Strategy> _strategy;
		bool _initialised;
		logger& _logger;

		void exit_on_key()
		{
			std::cin.get();
			std::exit(-1);
		}

	public:
		explicit constexpr runner(
			std::unique_ptr<internal::runner_implementation<Strategy>> implementation,
			runner_config config)
			: 
			_implementation{ std::move(implementation) }, 
			_config{ std::move(config) },
			_initialised{ false },
			_logger{ logger::instance() }
		{}

		template<typename... Args>
		void initialise(Args&&... args) noexcept
		{
			try
			{
				_logger.info("Starting initialisation...");

				_strategy = std::make_unique<Strategy>(std::forward<Args>(args)...);

				std::vector<std::shared_ptr<exchange>> exchanges = _implementation->create_exchanges(_config);
				_strategy->initialise(std::move(exchanges));

				_logger.info("Initialisation complete");
				_initialised = true;
			}
			catch (const std::exception& e)
			{
				_logger.critical("An error has occurred during initialisation which cannot be recovered from. The program will now terminate. Details: \n{}", e.what());
				exit_on_key();
			}
			
		}

		void run() noexcept
		{
			assert(_initialised);

			try
			{
				_implementation->run(*_strategy);
			}
			catch (const std::exception& e)
			{
				_logger.critical("An error has occurred which cannot be recovered from. The program will now terminate. Details: \n{}", e.what());
			}
			
			exit_on_key();
		}
	};

	template<typename Strategy>
	runner<Strategy> create_runner()
	{
		log_version();
		create_config_directory_if_not_exist();

		runner_config config = load_or_create_config<runner_config>();

		log_run_mode(config.runmode());

		std::unique_ptr<internal::runner_implementation<Strategy>> implementation;
		
		if (config.runmode() == run_mode::BACKTEST)
		{
			implementation = internal::create_backtest_runner<Strategy>();
		}
		else
		{
			implementation = std::make_unique<internal::default_runner<Strategy>>();
		}

		return runner<Strategy>
		{
			std::move(implementation),
			std::move(config)
		};
	}
}