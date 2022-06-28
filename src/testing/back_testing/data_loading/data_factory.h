#pragma once

#include "testing/back_testing/back_testing_data.h"
#include "testing/back_testing/back_testing_config.h"
#include "exchanges/exchange.h"

namespace mb
{
	std::shared_ptr<back_testing_data> load_back_testing_data(const back_testing_config& config);
}