#pragma once

#include <unordered_map>

#include "back_testing_data.h"
#include "testing/paper_trading/paper_trade_api.h"

namespace mb
{
	class asset_report
	{
	private:
		std::string _asset;
		std::string _start;
		std::string _end;
		std::string _change;
		std::string _percentageChange;
		std::string _annualReturn;

	public:
		constexpr asset_report(
			std::string asset,
			std::string start,
			std::string end,
			std::string change,
			std::string percentageChange,
			std::string annualReturn)
			: 
			_asset{ std::move(asset) },
			_start{ std::move(start) },
			_end{ std::move(end) },
			_change{ std::move(change) },
			_percentageChange{ std::move(percentageChange) },
			_annualReturn{ std::move(annualReturn) }
		{}

		constexpr const std::string& asset() const noexcept { return _asset; }
		constexpr const std::string& start_balance() const noexcept { return _start; }
		constexpr const std::string& end_balance() const noexcept { return _end; }
		constexpr const std::string& change() const noexcept { return _change; }
		constexpr const std::string& percentage_change() const noexcept { return _percentageChange; }
		constexpr const std::string& annual_return() const noexcept { return _annualReturn; }
	};

	class back_testing_report
	{
	private:
		std::string _startTime;
		std::string _endTime;
		std::string _stepSize;
		std::string _timeSteps;
		std::string _tradesCount;
		std::vector<asset_report> _assetReports;

	public:
		constexpr back_testing_report(
			std::string startTime,
			std::string endTime,
			std::string stepSize,
			std::string timeSteps,
			std::string tradesCount,
			std::vector<asset_report> assetReports)
			: 
			_startTime{ std::move(startTime) },
			_endTime{ std::move(endTime) },
			_stepSize{ std::move(stepSize) },
			_timeSteps{ std::move(timeSteps) },
			_tradesCount{ std::move(tradesCount) },
			_assetReports{ std::move(assetReports) }
		{}

		constexpr const std::string& start_time() const noexcept { return _startTime; }
		constexpr const std::string& end_time() const noexcept { return _endTime; }
		constexpr const std::string& step_size() const noexcept { return _stepSize; }
		constexpr const std::string& time_steps() const noexcept { return _timeSteps; }
		constexpr const std::string& trades_count() const noexcept { return _tradesCount; }
		constexpr const std::vector<asset_report>& asset_reports() const noexcept { return _assetReports; }
	};

	back_testing_report generate_back_testing_report(
		const back_testing_data& backTestingData,
		const unordered_string_map<double>& initialBalances,
		std::shared_ptr<paper_trade_api> paperTradeApi);

	std::string generate_report_string(const back_testing_report& report);
}