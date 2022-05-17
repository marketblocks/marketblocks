#include <unordered_set>

#include "back_testing_report.h"
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
}

namespace mb
{
	back_testing_report generate_back_testing_report(
		const back_testing_data& backTestingData,
		const unordered_string_map<double>& initialBalances, 
		std::shared_ptr<paper_trade_api> paperTradeApi)
	{
		static constexpr std::string_view DATE_FORMAT = "%d-%m-%Y %H:%M:%S";

		return back_testing_report
		{ 
			to_string(backTestingData.start_time(), DATE_FORMAT),
			to_string(backTestingData.end_time(), DATE_FORMAT),
			std::to_string(backTestingData.step_size()),
			std::to_string(backTestingData.time_steps()),
			std::to_string(paperTradeApi->get_closed_orders().size()),
			create_asset_reports(backTestingData.start_time(), backTestingData.end_time(), initialBalances, paperTradeApi)
		};
	}

	std::string generate_report_string(const back_testing_report& report)
	{
		static constexpr std::string_view INDENT = "    ";

		std::stringstream stream;
		stream << "Back Test Report" << std::endl;
		stream << "--------------------" << std::endl;
		stream << "Start Time: " << report.start_time() << std::endl;
		stream << "End Time: " << report.end_time() << std::endl;
		stream << "Step Size: " << report.step_size() << std::endl;
		stream << "Time Steps: " << report.time_steps() << std::endl;
		stream << "Total Number of Trades: " << report.trades_count() << std::endl;
		stream << std::endl;
		stream << "Asset Reports" << std::endl;
		stream << "--------------------" << std::endl;

		for (auto& assetReport : report.asset_reports())
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
}