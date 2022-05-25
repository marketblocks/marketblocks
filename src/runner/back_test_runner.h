#pragma once

#include "runner_implementation.h"
#include "testing/back_testing/backtest_market_api.h"
#include "testing/back_testing/back_testing_config.h"
#include "testing/paper_trading/paper_trade_api.h"
#include "testing/reporting/back_test_report.h"
#include "logging/logger.h"
#include "common/utils/mathutils.h"

namespace mb::internal
{
	std::shared_ptr<backtest_market_api> create_backtest_market_api(const back_testing_config& config);
	std::shared_ptr<paper_trade_api> create_paper_trade_api();
	
	template<typename Strategy>
	class back_test_runner : public runner_implementation<Strategy>
	{
	private:
		back_testing_config _config;
		std::shared_ptr<back_test_exchange> _backTestExchange;

	public:
		back_test_runner(back_testing_config config)
			: _config{ std::move(config) }
		{}

		std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig) override
		{
			std::shared_ptr<backtest_market_api> marketApi{ create_backtest_market_api(_config) };
			std::shared_ptr<paper_trade_api> tradeApi{ create_paper_trade_api() };

			_backTestExchange = std::make_shared<back_test_exchange>(marketApi, tradeApi);

			return
			{
				_backTestExchange
			};
		}

		void run(Strategy& strategy) override
		{
			std::shared_ptr<backtest_market_api> backTestMarketApi = _backTestExchange->market_api();
			std::shared_ptr<paper_trade_api> paperTradeApi = _backTestExchange->trade_api();

			const back_testing_data& data = backTestMarketApi->get_back_testing_data();
			int timeSteps = data.time_steps();

			unordered_string_map<double> initialBalances = paperTradeApi->get_balances();

			int lastLoggedPercentage = -1;
			auto startTime = std::chrono::system_clock::now();

			for (int i = 0; i < timeSteps; ++i)
			{
				int percentageComplete = calculate_percentage_proportion(1, timeSteps, i + 1);
				
				if (percentageComplete > lastLoggedPercentage)
				{
					logger::instance().info("Running back test {}% complete", percentageComplete);
					lastLoggedPercentage = percentageComplete;
				}

				try
				{
					strategy.run_iteration();
				}
				catch (const mb_exception& e)
				{
					logger::instance().error(e.what());
				}

				backTestMarketApi->increment_data();
			}

			auto endTime = std::chrono::system_clock::now();
			auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);

			logger::instance().info("Back test complete. Generating report...");

			back_test_report report{ generate_back_test_report(data, initialBalances, paperTradeApi, elapsedTime) };
			log_test_results(report);
		}
	};

	template<typename Strategy>
	std::unique_ptr<back_test_runner<Strategy>> create_back_test_runner()
	{
		back_testing_config config = load_or_create_config<back_testing_config>();
		return std::make_unique<back_test_runner<Strategy>>(std::move(config));
	}
}