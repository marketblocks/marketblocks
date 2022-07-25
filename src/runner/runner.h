#pragma once

#include <stdexcept>

#include "runner_implementation.h"
#include "live_runner.h"
#include "live_test_runner.h"
#include "back_test_runner.h"
#include "runner_config.h"
#include "common/file/config_file_reader.h"
#include "system/time_synchronization.h"
#include "logging/logger.h"

namespace mb
{
	namespace internal
	{
		void log_version();
		void log_run_mode(run_mode runMode);
		void exit_on_key();
		std::unique_ptr<internal::time_synchronizer> create_synchronizer_if_enabled(run_mode runMode, bool enabled);

		template<typename Strategy>
		std::unique_ptr<internal::runner_implementation<Strategy>> create_runner_implementation(run_mode runMode)
		{
			switch (runMode)
			{
			case run_mode::LIVE:
				return create_live_runner<Strategy>();
			case run_mode::LIVETEST:
				return create_live_test_runner<Strategy>();
			case run_mode::BACKTEST:
				return create_back_test_runner<Strategy>();
			default:
				throw std::invalid_argument{ "Run mode is not supported" };
			}
		}
	}

	template<typename Strategy>
	class runner
	{
	private:
		std::unique_ptr<internal::runner_implementation<Strategy>> _implementation;
		runner_config _config;
		bool _initialised;
		logger& _logger;
		std::unique_ptr<internal::time_synchronizer> _timeSynchronizer;

		std::unique_ptr<Strategy> _strategy;

	public:
		explicit runner(
			std::unique_ptr<internal::runner_implementation<Strategy>> implementation,
			runner_config config)
			:
			_implementation{ std::move(implementation) },
			_config{ std::move(config) },
			_initialised{ false },
			_logger{ logger::instance() },
			_timeSynchronizer{ internal::create_synchronizer_if_enabled(_config.runmode(), _config.sync_time()) }
		{}

		template<typename Config>
		Config load_custom_config()
		{
			try
			{
				return internal::load_or_create_config<Config>();
			}
			catch (const std::exception& e)
			{
				_logger.critical("Error occurred loading custom config file: {}", e.what());
				internal::exit_on_key();
			}
		}

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
				internal::exit_on_key();
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
			
			internal::exit_on_key();
		}
	};

	template<typename Strategy>
	runner<Strategy> create_runner()
	{
		try
		{
			internal::log_version();
			internal::create_config_directory_if_not_exist();

			runner_config config = internal::load_or_create_config<runner_config>();

			internal::log_run_mode(config.runmode());

			auto implementation = internal::create_runner_implementation<Strategy>(config.runmode());

			return runner<Strategy>
			{
				std::move(implementation),
				std::move(config)
			};
		}
		catch (const std::exception& e)
		{
			logger::instance().critical(e.what());
			internal::exit_on_key();
		}
	}
}