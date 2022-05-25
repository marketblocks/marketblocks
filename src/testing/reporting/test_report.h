#pragma once

#include <vector>
#include <chrono>

#include "asset_report.h"
#include "common/types/unordered_string_map.h"
#include "testing/back_testing/back_testing_data.h"
#include "testing/paper_trading/paper_trade_api.h"

namespace mb
{
	using report_result_list = std::vector<std::pair<std::string, std::string>>;

	class generic_test_results
	{
	private:
		std::string _elapsedTime;
		std::string _startTime;
		std::string _endTime;
		std::string _tradesCount;
		std::vector<asset_report> _assetReports;
		const std::vector<order_description>& _trades;

	public:
		constexpr generic_test_results(
			std::string elapsedTime,
			std::string startTime,
			std::string endTime,
			std::string tradesCount,
			std::vector<asset_report> assetReports,
			const std::vector<order_description>& trades)
			:
			_elapsedTime{ std::move(elapsedTime) },
			_startTime{ std::move(startTime) },
			_endTime{ std::move(endTime) },
			_tradesCount{ std::move(tradesCount) },
			_assetReports{ std::move(assetReports) },
			_trades{ trades }
		{}

		constexpr const std::string& elapsed_time() const noexcept { return _elapsedTime; }
		constexpr const std::string& start_time() const noexcept { return _startTime; }
		constexpr const std::string& end_time() const noexcept { return _endTime; }
		constexpr const std::string& trades_count() const noexcept { return _tradesCount; }
		constexpr const std::vector<asset_report>& asset_reports() const noexcept { return _assetReports; }
		constexpr const std::vector<order_description>& trades() const noexcept { return _trades; }
	};

	class test_report
	{
	private:
		generic_test_results _genericResults;

	public:
		virtual ~test_report() = default;

		constexpr test_report(generic_test_results genericResults)
			: _genericResults{ std::move(genericResults) }
		{}

		const generic_test_results& get_generic_results() const noexcept { return _genericResults; }
		virtual report_result_list get_specific_results() const = 0;
	};

	generic_test_results create_generic_results(
		std::chrono::milliseconds elapsedTime,
		std::time_t startTime,
		std::time_t endTime,
		const unordered_string_map<double>& initialBalances,
		std::shared_ptr<paper_trade_api> paperTradeApi);

	void log_test_results(const test_report& report);
}