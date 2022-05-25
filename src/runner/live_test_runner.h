#pragma once

#include "runner_implementation.h"
#include "exchange_factory.h"
#include "logging/logger.h"

namespace mb::internal
{
	template<typename Strategy>
	class live_test_runner : public runner_implementation<Strategy>
	{
	private:
		std::vector<std::shared_ptr<exchange>> _liveTestExchanges;

	public:
		std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig) override
		{
			std::vector<std::shared_ptr<exchange>> exchangeApis{ create_exchange_apis(runnerConfig) };
			_liveTestExchanges.reserve(exchangeApis.size());

			paper_trading_config paperTradingConfig{ load_or_create_config<paper_trading_config>() };

			for (auto exchange : exchangeApis)
			{
				_liveTestExchanges.emplace_back(
					std::make_shared<live_test_exchange>(exchange, std::make_shared<paper_trade_api>(paperTradingConfig)));
			}

			return _liveTestExchanges;
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

	template<typename Strategy>
	std::unique_ptr<live_test_runner<Strategy>> create_live_test_runner()
	{
		return std::make_unique<live_test_runner<Strategy>>();
	}
}