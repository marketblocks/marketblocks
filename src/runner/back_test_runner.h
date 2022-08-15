#pragma once

#include "runner_implementation.h"
#include "testing/back_testing/back_test_market_api.h"
#include "testing/back_testing/back_testing_config.h"
#include "testing/back_testing/data_loading/data_factory.h"
#include "testing/paper_trading/paper_trade_api.h"
#include "testing/reporting/back_test_report.h"
#include "testing/reporting/test_logger.h"
#include "logging/logger.h"
#include "common/utils/mathutils.h"
#include "exchanges/multi_component_exchange.h"
#include "exchanges/exchange_ids.h"

namespace mb::internal
{
	template<typename Strategy>
	class back_test_runner : public runner_implementation<Strategy>
	{
	private:
		back_testing_config _config;
		std::shared_ptr<back_testing_data> _backTestingData;
		std::shared_ptr<backtest_websocket_stream> _websocketStream;
		std::shared_ptr<paper_trade_api> _paperTradeApi;

	public:
		back_test_runner(back_testing_config config)
			: _config{ std::move(config) }
		{}

		std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig) override
		{
			_backTestingData = load_back_testing_data(
				create_data_source(_config.data_directory()),
				_config);

			_websocketStream = std::make_shared<backtest_websocket_stream>(_backTestingData);
			
			_paperTradeApi = create_paper_trade_api(
				exchange_ids::BACK_TEST, 
				_websocketStream,
				[this]() { return _backTestingData->data_time(); });

			return
			{
				std::make_shared<back_test_exchange>(
					exchange_ids::BACK_TEST,
					_websocketStream,
					std::make_shared<back_test_market_api>(_backTestingData),
					_paperTradeApi)
			};
		}

		void run(Strategy& strategy) override
		{
			test_logger testLogger{ mb::create_test_logger({ _paperTradeApi }) };
			int timeSteps{ _backTestingData->time_steps() };
			int lastLoggedPercentage = -1;

			for (int i = 0; i < timeSteps; ++i)
			{
				int percentageComplete = calculate_percentage_proportion(1, timeSteps, i + 1);
				
				if (percentageComplete > lastLoggedPercentage)
				{
					logger::instance().info("Running back test {}% complete", percentageComplete);
					lastLoggedPercentage = percentageComplete;
				}

				_websocketStream->notify();

				try
				{
					strategy.run_iteration();
					testLogger.flush_trades();
				}
				catch (const mb_exception& e)
				{
					logger::instance().error(e.what());
				}

				_backTestingData->increment();
			}

			logger::instance().info("Back test complete. Generating report...");

			test_report report{ generate_back_test_report(*_backTestingData, testLogger, strategy.get_test_results()) };
			testLogger.log_test_report(report);
		}
	};

	template<typename Strategy>
	std::unique_ptr<back_test_runner<Strategy>> create_back_test_runner()
	{
		back_testing_config config{ load_or_create_config<back_testing_config>() };
		return std::make_unique<back_test_runner<Strategy>>(std::move(config));
	}
}