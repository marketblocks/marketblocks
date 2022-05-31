#pragma once

#include "runner_implementation.h"
#include "exchange_factory.h"
#include "logging/logger.h"

namespace mb::internal
{
	template<typename Strategy>
	class live_runner : public runner_implementation<Strategy>
	{
	private:
		int _runInterval;

	public:
		std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig) override
		{
			_runInterval = runnerConfig.run_interval();

			return create_exchange_apis(runnerConfig);
		}

		void run(Strategy& strategy) override
		{
			while (true)
			{
				try
				{
					strategy.run_iteration();
				}
				catch (const mb_exception& e)
				{
					logger::instance().error(e.what());
				}

				if (_runInterval > 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(_runInterval));
				}
			}
		}
	};

	template<typename Strategy>
	std::unique_ptr<live_runner<Strategy>> create_live_runner()
	{
		return std::make_unique<live_runner<Strategy>>();
	}
}