#pragma once

#include "back_testing_data.h"
#include "back_testing_config.h"
#include "exchanges/exchange.h"

namespace mb
{
	back_testing_data load_back_testing_data(const back_testing_config& config, std::shared_ptr<exchange> exchange);
}