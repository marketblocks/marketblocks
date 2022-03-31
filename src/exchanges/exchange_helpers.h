#pragma once

#include "exchange.h"

namespace cb
{
	double get_balance(std::shared_ptr<exchange> exchange, std::string_view tickerId);
}