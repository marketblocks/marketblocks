#pragma once

#include <stdexcept>
#include <spdlog/spdlog.h>

#include "initialise/runner_initialise.h"
#include "common/file/config_file_reader.h"
#include "logging/logger.h"

namespace cb
{
	template<typename Strategy>
	class runner
	{
	private:
		bool _initialised;
		Strategy _strategy;
		logger& _logger;

	public:
		template<typename... Args>
		explicit constexpr runner(Args&&... args)
			: _initialised{ false }, _strategy{ std::forward<Args>(args)... }, _logger{ logger::instance() }
		{
		}

		void initialise()
		{
			_logger.info("Starting initialisation...");

			create_config_directory_if_not_exist();

			runner_config runnerConfig = internal::get_runner_config();
			std::vector<std::shared_ptr<exchange>> exchanges = internal::create_exchanges(runnerConfig);

			strategy_initialiser strategyInitialiser
			{
				std::move(exchanges)
			};
			
			_logger.info("Initialising strategy...");
				
			_strategy.initialise(strategyInitialiser);

			_logger.info("Initialisation complete");
			_initialised = true;
		}

		void run()
		{
			if (!_initialised)
			{
				throw cb_exception{ "Runner must be initialized" };
			}

			while (true)
			{
				try
				{
					_strategy.run_iteration();
				}
				catch (const std::exception& e)
				{
					_logger.error(e.what());
				}
			}
		}
	};
}