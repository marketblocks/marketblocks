#include "back_test_report.h"
#include "test_logger.h"
#include "common/utils/timeutils.h"

namespace mb
{
	test_report generate_back_test_report(
		const back_testing_data& backTestingData,
		const test_logger& testLogger,
		report_result_list strategyResults)
	{
		report_result_list additionalResults
		{
			{ "Data Start Time", to_string(backTestingData.start_time(), DATE_TIME_FORMAT) },
			{ "Data End Time", to_string(backTestingData.end_time(), DATE_TIME_FORMAT) },
			{ "Step Size", std::to_string(backTestingData.step_size()) },
			{ "Time Steps", std::to_string(backTestingData.time_steps()) }
		};

		additionalResults.insert(additionalResults.end(), strategyResults.begin(), strategyResults.end());

		std::time_t dataTimeRange = backTestingData.end_time() - backTestingData.start_time();

		return testLogger.generate_test_report(dataTimeRange, std::move(additionalResults));
	}
}