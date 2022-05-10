#pragma once

#include "runner_implementation.h"
#include "testing/back_testing/backtest_market_api.h"
#include "testing/paper_trading/paper_trade_api.h"
#include "logging/logger.h"

namespace mb::internal
{
	std::shared_ptr<backtest_market_api> create_backtest_market_api();
	std::shared_ptr<paper_trade_api> create_paper_trade_api();

	template<typename Strategy>
	class backtest_runner : public runner_implementation<Strategy>
	{
	private:
		std::shared_ptr<backtest_market_api> _backtestMarketApi;
		std::shared_ptr<paper_trade_api> _paperTradeApi;

	public:
		std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig) override
		{
			_backtestMarketApi = create_backtest_market_api();
			_paperTradeApi = create_paper_trade_api();

			return
			{
				std::make_shared<back_test_exchange>(_backtestMarketApi, _paperTradeApi)
			};
		}

		void run(Strategy& strategy) override
		{
			int dataSize = _backtestMarketApi->get_back_testing_data().size();

			for (int i = 0; i < dataSize; ++i)
			{
				logger::instance().info("Running back test iteration {0}/{1}", i, dataSize);

				strategy.run_iteration();
				
				_backtestMarketApi->increment_data();
			}
		}
	};
}