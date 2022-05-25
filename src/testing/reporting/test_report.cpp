#include <unordered_set>

#include "test_report.h"
#include "logging/logger.h"
#include "common/utils/timeutils.h"
#include "common/utils/containerutils.h"
#include "common/utils/mathutils.h"

namespace
{
	using namespace mb;

	std::unordered_set<std::string_view> get_unique_assets(
		const unordered_string_map<double>& initialBalances,
		const unordered_string_map<double>& finalBalances)
	{
		std::unordered_set<std::string_view> assets;

		for (auto& [asset, _] : initialBalances)
		{
			assets.emplace(asset);
		}

		for (auto& [asset, _] : finalBalances)
		{
			assets.emplace(asset);
		}

		return assets;
	}

	std::vector<asset_report> create_asset_reports(
		std::time_t startTime,
		std::time_t endTime,
		const unordered_string_map<double>& initialBalances,
		std::shared_ptr<paper_trade_api> paperTradeApi)
	{
		constexpr int PRECISION = 2;

		unordered_string_map<double> finalBalances{ paperTradeApi->get_balances() };

		std::unordered_set<std::string_view> assets = get_unique_assets(initialBalances, finalBalances);

		std::vector<asset_report> assetReports;
		assetReports.reserve(assets.size());

		std::string percentageChange;
		std::string annualReturn;

		for (auto& asset : assets)
		{
			double startBalance = find_or_default(initialBalances, asset, 0.0);
			double finalBalance = find_or_default(finalBalances, asset, 0.0);
			double change = finalBalance - startBalance;

			if (startBalance == 0.0)
			{
				percentageChange = annualReturn = "N/A";
			}
			else
			{
				double pChange = calculate_percentage_diff(startBalance, finalBalance);
				double years = (endTime - startTime) / 31536000.0;

				percentageChange = to_string(pChange, PRECISION) + "%";
				annualReturn = to_string(pChange / years, PRECISION) + "%";
			}

			assetReports.emplace_back(
				std::string{ asset },
				to_string(startBalance, PRECISION),
				to_string(finalBalance, PRECISION),
				to_string(change, PRECISION),
				percentageChange,
				annualReturn);
		}

		return assetReports;
	}

	std::string generate_report_string(const test_report& report)
	{
		static constexpr std::string_view INDENT = "    ";
		const generic_test_results& genericResults = report.get_generic_results();

		std::stringstream stream;
		stream << "Back Test Report" << std::endl;
		stream << "--------------------" << std::endl;
		stream << "Elapsed Time: " << genericResults.elapsed_time() << std::endl;
		stream << "Total Number of Trades: " << genericResults.trades_count() << std::endl;

		for (auto& [resultName, resultValue] : report.get_specific_results())
		{
			stream << resultName << ": " << resultValue << std::endl;
		}

		stream << std::endl;
		stream << "Asset Reports" << std::endl;
		stream << "--------------------" << std::endl;

		for (auto& assetReport : genericResults.asset_reports())
		{
			stream << assetReport.asset() << ":" << std::endl;
			stream << INDENT << "Starting Balance: " << assetReport.start_balance() << std::endl;
			stream << INDENT << "End Balance: " << assetReport.end_balance() << std::endl;
			stream << INDENT << "Change: " << assetReport.change() << std::endl;
			stream << INDENT << "Percentage Change: " << assetReport.percentage_change() << std::endl;
			stream << INDENT << "Annual Return: " << assetReport.annual_return() << std::endl;
			stream << std::endl;
		}

		return stream.str();
	}

	std::filesystem::path get_output_path()
	{
		static constexpr std::string_view outputDirectory = "test_results";

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

namespace mb
{
	generic_test_results create_generic_results(
		std::chrono::milliseconds elapsedTime,
		std::time_t startTime,
		std::time_t endTime,
		const unordered_string_map<double>& initialBalances,
		std::shared_ptr<paper_trade_api> paperTradeApi)
	{
		static constexpr std::string_view DATE_TIME_FORMAT = "%d-%m-%Y %H:%M:%S";

		return generic_test_results
		{
			std::to_string(elapsedTime.count()) + "ms",
			to_string(startTime, DATE_TIME_FORMAT),
			to_string(endTime, DATE_TIME_FORMAT),
			std::to_string(paperTradeApi->get_closed_orders().size()),
			create_asset_reports(startTime, endTime, initialBalances, paperTradeApi),
			paperTradeApi->get_closed_orders()
		};
	}

	void log_test_results(const test_report& report)
	{
		std::string reportString{ generate_report_string(report) };

		std::filesystem::path outputPath = get_output_path();
		logger::instance().info("Writing results to {}", outputPath.string());

		save_results_to_file(reportString, report.get_generic_results().trades(), outputPath);

		logger::instance().info("\n" + reportString);
	}
}