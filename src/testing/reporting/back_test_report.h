#pragma once

#include "test_report.h"
#include "test_logger.h"
#include "testing/back_testing/back_testing_data.h"

namespace mb
{
	test_report generate_back_test_report(
		const back_testing_data& backTestingData,
		const test_logger& testLogger);
}