#pragma once

#include <vector>
#include <chrono>

#include "asset_report.h"
#include "testing/back_testing/back_testing_data.h"
#include "testing/paper_trading/paper_trade_api.h"

namespace mb
{
	using report_result_list = std::vector<std::pair<std::string, std::string>>;

	static constexpr std::string_view DATE_TIME_FORMAT = "%d-%m-%Y %H:%M:%S";

	class test_report
	{
	private:
		std::string _elapsedTime;
		std::string _startTime;
		std::string _endTime;
		std::string _tradesCount;
		std::vector<asset_report> _assetReports;
		report_result_list _additionalResults;

	public:
		virtual ~test_report() = default;

		test_report(
			std::string elapsedTime,
			std::string startTime,
			std::string endTime,
			std::string tradesCount,
			std::vector<asset_report> assetReports,
			report_result_list additionalResults);

		const std::string& elapsed_time() const noexcept { return _elapsedTime; }
		const std::string& start_time() const noexcept { return _startTime; }
		const std::string& end_time() const noexcept { return _endTime; }
		const std::string& trades_count() const noexcept { return _tradesCount; }
		const std::vector<asset_report>& asset_reports() const noexcept { return _assetReports; }
		const report_result_list& get_additional_results() const noexcept { return _additionalResults; };
	};

	std::string generate_report_string(const test_report& report);
}