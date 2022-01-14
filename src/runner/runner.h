#pragma once

#include <stdexcept>

#include "initialise/runner_initialise.h"

namespace cb
{
	template<typename Strategy>
	class runner
	{
	private:
		run_mode _runMode;
		bool _initialised;
		Strategy _strategy;

	public:
		template<typename... Args>
		explicit runner(run_mode runMode, Args&&... args)
			: _runMode{ runMode }, _initialised{ false }, _strategy{ std::forward<Args>(args)... }
		{
		}

		void initialise()
		{
			runner_config runnerConfig = internal::get_runner_config();
			trading_options tradingOptions = internal::get_trading_options();
			std::vector<std::shared_ptr<exchange>> exchanges = internal::create_exchanges(runnerConfig, _runMode);

			strategy_initialiser strategyInitialiser
			{
				std::move(exchanges),
				std::move(tradingOptions)
			};

			_strategy.initialise(strategyInitialiser);

			_initialised = true;
		}

		void run()
		{
			if (!_initialised)
			{
				throw std::runtime_error("Runner must be initialized");
			}

			while (true)
			{
				_strategy.run_iteration();
			}
		}
	};
}