#include "test_logger.h"
#include "common/utils/timeutils.h"
#include "common/utils/mathutils.h"
#include "common/utils/containerutils.h"
#include "logging/logger.h"

namespace
{
	using namespace mb;

	std::filesystem::path get_output_path()
	{
		static constexpr std::string_view outputDirectory = "test_results";

		std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string resultsFolderName = to_string(time, "%d%m%Y_%H%M%S");

		std::filesystem::path path{ outputDirectory };
		path /= resultsFolderName;

		return path;
	}

	file_handler create_trades_file(std::filesystem::path path)
	{
		static constexpr std::string_view TRADES_FILENAME = "trades.csv";

		std::vector<std::string> tradesCsvHeaders
		{
			"Time Stamp", "Order ID", "Market", "Action", "Price", "Volume"
		};

		file_handler tradeFileHandler{ file_handler::write(path / TRADES_FILENAME) };

		tradeFileHandler.stream() << csv_row{ tradesCsvHeaders }.to_string() << std::endl;

		return tradeFileHandler;
	}

	void create_report_file(std::string_view report, const std::filesystem::path& path)
	{
		static constexpr std::string_view REPORT_FILENAME = "report.txt";

		try
		{
			write_to_file(path / REPORT_FILENAME, report);
		}
		catch (const std::exception& e)
		{
			logger::instance().error("An error occurred whilst saving back test results: {}", e.what());
		}
	}

	std::vector<asset_report> create_asset_reports(
		std::time_t dataTimeRange,
		std::vector<test_logger_exchange_data> exchangeData)
	{
		constexpr int PRECISION = 8;

		std::unordered_set<std::string> uniqueAssets;
		std::unordered_map<std::string,double> initialBalances;
		std::unordered_map<std::string,double> finalBalances;

		for (auto& exchange : exchangeData)
		{
			for (auto& [asset, balance] : exchange.initial_balances())
			{
				initialBalances[asset] += balance;
				uniqueAssets.emplace(asset);
			}

			for (auto& [asset, balance] : exchange.trade_api()->get_balances())
			{
				finalBalances[asset] += balance;
				uniqueAssets.emplace(asset);
			}
		}

		std::vector<asset_report> assetReports;
		assetReports.reserve(uniqueAssets.size());

		std::string percentageChange;
		std::string annualReturn;

		for (auto& asset : uniqueAssets)
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
				double years = dataTimeRange / 31536000.0;

				percentageChange = to_string(pChange, 2) + "%";
				annualReturn = to_string(pChange / years, 2) + "%";
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
}

namespace mb
{
	test_logger_exchange_data::test_logger_exchange_data(std::shared_ptr<paper_trade_api> tradeApi)
		: _tradeApi{ tradeApi }, _initialBalances{ _tradeApi->get_balances() }, _closedOrderIndex{ 0 }
	{}

	test_logger::test_logger(
		std::vector<test_logger_exchange_data> exchangeData,
		std::filesystem::path outputDirectory,
		file_handler tradeFileHandler)
		: 
		_exchangeData{ std::move(exchangeData) },
		_outputDirectory{ std::move(outputDirectory) },
		_tradeFileHandler{ std::move(tradeFileHandler) },
		_startTime{ now_t() }
	{}

	void test_logger::flush_trades()
	{
		for (auto& exchange : _exchangeData)
		{
			auto tradeApi = exchange.trade_api();

			tradeApi->fill_open_orders();
			std::vector<order_description> closedOrders{ tradeApi->get_closed_orders() };

			if (closedOrders.empty())
			{
				continue;
			}

			int nextTrade = exchange.closed_order_index();

			for (; nextTrade < closedOrders.size(); ++nextTrade)
			{
				_tradeFileHandler.stream() << to_csv_row<order_description>(closedOrders[nextTrade]).to_string() << std::endl;
			}

			exchange.set_closed_order_index(nextTrade);
		}
	}

	test_report test_logger::generate_test_report(std::time_t dataTimeRange, report_result_list additionalResults) const
	{
		std::time_t endTime{ now_t() };
		std::time_t elapsedTime = endTime - _startTime;

		if (dataTimeRange == 0)
		{
			dataTimeRange = endTime - _startTime;
		}

		int numberOfTrades = std::accumulate(_exchangeData.begin(), _exchangeData.end(), 0,
			[](int i, const test_logger_exchange_data& data) { return data.trade_api()->get_closed_orders().size(); });

		return test_report
		{
			std::to_string(elapsedTime) + "s",
			to_string(_startTime, DATE_TIME_FORMAT),
			to_string(endTime, DATE_TIME_FORMAT),
			std::to_string(numberOfTrades),
			create_asset_reports(dataTimeRange, _exchangeData),
			std::move(additionalResults)
		};
	}

	void test_logger::log_test_report(const test_report& report) const
	{
		std::string reportString{ generate_report_string(report) };

		create_report_file(reportString, _outputDirectory);

		logger::instance().info("\n" + reportString);
	}

	test_logger create_test_logger(std::vector<std::shared_ptr<paper_trade_api>> tradeApis)
	{
		std::filesystem::path path{ get_output_path() };
		std::filesystem::create_directories(path);

		logger::instance().info("Test results will be written to {}", path.string());

		file_handler tradeFileHandler{ create_trades_file(path) };

		std::vector<test_logger_exchange_data> exchangeData;
		exchangeData.reserve(tradeApis.size());

		for (auto tradeApi : tradeApis)
		{
			exchangeData.emplace_back(tradeApi);
		}

		return test_logger{ std::move(exchangeData), std::move(path), std::move(tradeFileHandler) };
	}
}