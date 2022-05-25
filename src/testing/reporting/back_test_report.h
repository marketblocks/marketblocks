#pragma once

#include "test_report.h"

namespace mb
{
	class back_test_report : public test_report
	{
	private:
		std::string _stepSize;
		std::string _timeSteps;

	public:
		back_test_report(
			generic_test_results genericResults,
			std::string stepSize,
			std::string timeSteps)
			: 
			test_report{ std::move(genericResults) },
			_stepSize{ std::move(stepSize) },
			_timeSteps{ std::move(timeSteps) }
		{}

		report_result_list get_specific_results() const override;
	};

	back_test_report generate_back_test_report(
		const back_testing_data& backTestingData,
		const unordered_string_map<double>& initialBalances,
		std::shared_ptr<paper_trade_api> paperTradeApi,
		std::chrono::milliseconds elapsedTime);
}