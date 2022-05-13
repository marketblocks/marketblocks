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
		backtest_runner(std::shared_ptr<backtest_market_api> backtestMarketApi, std::shared_ptr<paper_trade_api> paperTradeApi)
			: _backtestMarketApi{ backtestMarketApi }, _paperTradeApi{ paperTradeApi }
		{}

		std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig) override
		{
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

				try
				{
					strategy.run_iteration();
				}
				catch (const mb_exception& e)
				{
					logger::instance().error(e.what());
				}

				_backtestMarketApi->increment_data();
			}
		}
	};

	template<typename Strategy>
	std::unique_ptr<backtest_runner<Strategy>> create_backtest_runner()
	{
		std::shared_ptr<backtest_market_api> backtestMarketApi = create_backtest_market_api();
		std::shared_ptr<paper_trade_api> paperTradeApi = create_paper_trade_api();

		return std::make_unique<backtest_runner<Strategy>>(backtestMarketApi, paperTradeApi);
	}
}