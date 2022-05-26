#include <unordered_set>

#include "test_report.h"

namespace mb
{
	std::string generate_report_string(const test_report& report)
	{
		static constexpr std::string_view INDENT = "    ";

		std::stringstream stream;
		stream << "Test Report" << std::endl;
		stream << "--------------------" << std::endl;
		stream << "Elapsed Time: " << report.elapsed_time() << std::endl;
		stream << "Start Time: " << report.start_time() << std::endl;
		stream << "End Time: " << report.end_time() << std::endl;
		stream << "Total Number of Trades: " << report.trades_count() << std::endl;

		for (auto& [resultName, resultValue] : report.get_additional_results())
		{
			stream << resultName << ": " << resultValue << std::endl;
		}

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