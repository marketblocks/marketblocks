#include <vector>
#include <format>

#include "tradable_pair.h"
#include "common/utils/stringutils.h"
#include "common/exceptions/mb_exception.h"

namespace mb
{
	tradable_pair parse_tradable_pair(std::string_view string, char seperator)
	{
		std::vector<std::string> assets{ split(string, seperator) };

		if (assets.size() != 2)
		{
			throw mb_exception{ std::format("Could parse tradable pair from {0} with seperator \"{1}\"", string, seperator) };
		}

		to_upper(assets[0]);
		to_upper(assets[1]);

		return tradable_pair{ assets[0], assets[1] };
	}
}