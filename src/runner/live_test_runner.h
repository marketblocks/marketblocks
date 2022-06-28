#pragma once

#include <atomic>

#include "runner_implementation.h"
#include "exchange_factory.h"
#include "testing/reporting/test_report.h"
#include "testing/reporting/test_logger.h"
#include "logging/logger.h"
#include "exchanges/multi_component_exchange.h"

namespace mb::internal
{
	test_logger create_test_logger(const std::vector<std::shared_ptr<live_test_exchange>>& liveTestExchanges);
	void check_for_stop(std::atomic_bool& run);

	template<typename Strategy>
	class live_test_runner : public runner_implementation<Strategy>
	{
	private:
		std::vector<std::shared_ptr<live_test_exchange>> _liveTestExchanges;
		std::atomic_bool _run;
		int _runInterval;

	public:
		std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig) override
		{
			_runInterval = runnerConfig.run_interval();

			std::vector<std::shared_ptr<exchange>> exchangeApis{ create_exchange_apis(runnerConfig) };
			_liveTestExchanges.reserve(exchangeApis.size());

			paper_trading_config paperTradingConfig{ load_or_create_config<paper_trading_config>() };

			for (auto exchange : exchangeApis)
			{
				auto paperTradeApi{ std::make_shared<paper_trade_api>(
					paperTradingConfig, 
					exchange->id(),
					[exchange](const tradable_pair& pair) { return exchange->get_price(pair); } )};

				_liveTestExchanges.emplace_back(
					std::make_shared<live_test_exchange>(exchange, paperTradeApi));
			}

			return std::vector<std::shared_ptr<exchange>>{ _liveTestExchanges.begin(), _liveTestExchanges.end() };
		}

		void run(Strategy& strategy) override
		{
			test_logger testLogger{ create_test_logger(_liveTestExchanges) };
			_run = true;

			std::thread stopThread{ check_for_stop, std::ref(_run) };

			while (_run)
			{
				try
				{
					strategy.run_iteration();
					testLogger.flush_trades();
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

			stopThread.join();

			logger::instance().info("Live test complete. Generating report...");

			test_report report{ testLogger.generate_test_report(0, strategy.get_test_results()) };
			testLogger.log_test_report(report);
		}
	};

	template<typename Strategy>
	std::unique_ptr<live_test_runner<Strategy>> create_live_test_runner()
	{
		return std::make_unique<live_test_runner<Strategy>>();
	}
}