#pragma once

#include "exchange.h"

namespace cb
{
	double get_balance(const exchange& exchange, std::string_view tickerId);
}