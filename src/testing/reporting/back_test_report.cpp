#include "back_test_report.h"
#include "common/utils/timeutils.h"

namespace mb
{
	report_result_list back_test_report::get_specific_results() const
	{
		return
		{
			{ "Step Size", _stepSize },
			{ "Time Steps", _timeSteps }
		};
	}

	back_test_report generate_back_test_report(
		const back_testing_data& backTestingData,
		const unordered_string_map<double>& initialBalances,
		std::shared_ptr<paper_trade_api> paperTradeApi,
		std::chrono::milliseconds elapsedTime)
	{
		generic_test_results genericResults{ create_generic_results(
			elapsedTime,
			backTestingData.start_time(),
			backTestingData.end_time(),
			initialBalances,
			paperTradeApi) };

		return back_test_report
		{
			std::move(genericResults),
			std::to_string(backTestingData.step_size()),
			std::to_string(backTestingData.time_steps()),
		};
	}
}