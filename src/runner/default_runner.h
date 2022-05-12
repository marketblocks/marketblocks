#pragma once

#include "runner_implementation.h"
#include "logging/logger.h"

namespace mb::internal
{
	std::vector<std::shared_ptr<exchange>> do_create_exchanges(const runner_config& runnerConfig);

	template<typename Strategy>
	class default_runner : public runner_implementation<Strategy>
	{
	public:
		std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig) override
		{
			return do_create_exchanges(runnerConfig);
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
			}
		}
	};
}