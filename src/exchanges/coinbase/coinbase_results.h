#pragma once

#include <vector>

#include "trading/tradable_pair.h"

namespace cb::internal
{
	std::vector<tradable_pair> read_tradable_pairs(std::string_view jsonResult);
}