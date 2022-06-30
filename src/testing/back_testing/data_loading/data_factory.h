#pragma once

#include "testing/back_testing/back_testing_data.h"
#include "testing/back_testing/back_testing_config.h"
#include "exchanges/exchange.h"

namespace mb
{
	std::unique_ptr<back_testing_data_source> create_data_source(std::string_view dataDirectory);
	std::shared_ptr<back_testing_data> load_back_testing_data(std::unique_ptr<back_testing_data_source> dataSource, const back_testing_config& config);
}