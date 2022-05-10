#pragma once

#include "runner_config.h"
#include "exchanges/exchange.h"

namespace mb::internal
{
	template<typename Strategy>
	class runner_implementation
	{
	public:
		virtual ~runner_implementation() = default;

		virtual std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig) = 0;
		virtual void run(Strategy& strategy) = 0;
	};
}