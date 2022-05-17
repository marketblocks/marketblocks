#include "backtest_runner.h"
#include "config_file_reader.h"
#include "testing/back_testing/data_loading.h"
#include "common/utils/timeutils.h"

namespace mb::internal
{
	std::shared_ptr<backtest_market_api> create_backtest_market_api(const back_testing_config& config)
	{
		back_testing_data data = load_back_testing_data(config);
		std::shared_ptr<back_testing_data_navigator> dataNavigator = std::make_shared<back_testing_data_navigator>(std::move(data));

		std::unique_ptr<backtest_websocket_stream> websocketStream{ std::make_unique<backtest_websocket_stream>(dataNavigator) };
		return std::make_shared<backtest_market_api>(dataNavigator, std::move(websocketStream));
	}

	std::shared_ptr<paper_trade_api> create_paper_trade_api()
	{
		paper_trading_config config = load_or_create_config<paper_trading_config>();

		return std::make_unique<paper_trade_api>(std::move(config));
	}

	std::filesystem::path get_full_output_path(std::string_view outputDirectory)
	{
		std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string resultsFolderName = to_string(time, "%d%m%Y_%H%M%S");

		std::filesystem::path path{ outputDirectory };
		path /= resultsFolderName;

		return path;
	}

	void save_results_to_file(std::string_view report, const std::vector<order_description>& orders, const std::filesystem::path& path)
	{
		static constexpr std::string_view TRADES_FILENAME = "trades.csv";
		static constexpr std::string_view REPORT_FILENAME = "report.txt";

		std::vector<std::string> tradesCsvHeaders
		{
			"Order ID", "Market", "Action", "Price", "Volume"
		};

		try
		{
			std::filesystem::create_directories(path);

			write_to_csv_file<order_description>(path / TRADES_FILENAME, orders, tradesCsvHeaders);
			write_to_file(path / REPORT_FILENAME, report);
		}
		catch (const std::exception& e)
		{
			logger::instance().error("An error occurred whilst saving back test results: {}", e.what());
		}
	}
}