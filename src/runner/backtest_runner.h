#pragma once

#include "runner_implementation.h"
#include "testing/back_testing/backtest_market_api.h"
#include "testing/back_testing/back_testing_report.h"
#include "testing/back_testing/back_testing_config.h"
#include "testing/paper_trading/paper_trade_api.h"
#include "logging/logger.h"
#include "common/utils/mathutils.h"

namespace mb::internal
{
	std::shared_ptr<backtest_market_api> create_backtest_market_api(const back_testing_config& config);
	std::shared_ptr<paper_trade_api> create_paper_trade_api();
	std::filesystem::path get_full_output_path(std::string_view outputDirectory);
	void save_results_to_file(std::string_view report, const std::vector<order_description>& orders, const std::filesystem::path& path);

	template<typename Strategy>
	class backtest_runner : public runner_implementation<Strategy>
	{
	private:
		std::shared_ptr<backtest_market_api> _backtestMarketApi;
		std::shared_ptr<paper_trade_api> _paperTradeApi;
		std::string _outputDirectory;

	public:
		backtest_runner(
			std::shared_ptr<backtest_market_api> backtestMarketApi, 
			std::shared_ptr<paper_trade_api> paperTradeApi,
			std::string outputDirectory)
			: 
			_backtestMarketApi{ backtestMarketApi }, 
			_paperTradeApi{ paperTradeApi },
			_outputDirectory{ std::move(outputDirectory) }
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
			const back_testing_data& data = _backtestMarketApi->get_back_testing_data();
			int timeSteps = data.time_steps();

			unordered_string_map<double> initialBalances = _paperTradeApi->get_balances();

			for (int i = 0; i < timeSteps; ++i)
			{
				int percentageComplete = calculate_percentage_proportion(1, timeSteps, i + 1);
				logger::instance().info("Running back test iteration {0}/{1} ({2}%)", i + 1, timeSteps, percentageComplete);

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

			logger::instance().info("Back test complete. Generating report...");

			back_testing_report report{ generate_back_testing_report(data, initialBalances, _paperTradeApi) };
			std::string reportString{ generate_report_string(report) };

			std::filesystem::path outputPath = get_full_output_path(_outputDirectory);
			logger::instance().info("Writing results to {}", outputPath.string());

			save_results_to_file(reportString, _paperTradeApi->get_closed_orders(), outputPath);

			logger::instance().info("\n" + reportString);
		}
	};

	template<typename Strategy>
	std::unique_ptr<backtest_runner<Strategy>> create_backtest_runner()
	{
		back_testing_config config = load_or_create_config<back_testing_config>();

		std::shared_ptr<backtest_market_api> backtestMarketApi = create_backtest_market_api(config);
		std::shared_ptr<paper_trade_api> paperTradeApi = create_paper_trade_api();

		return std::make_unique<backtest_runner<Strategy>>(backtestMarketApi, paperTradeApi, config.output_directory());
	}
}