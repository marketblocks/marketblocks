#include <vector>
#include <fmt/format.h>

#include "tradable_pair.h"
#include "common/utils/stringutils.h"
#include "common/exceptions/mb_exception.h"

namespace mb
{
	tradable_pair::tradable_pair(std::string asset, std::string priceUnit)
		:
		_asset{ std::move(asset) },
		_priceUnit{ std::move(priceUnit) }
	{}

	bool tradable_pair::contains(std::string_view assetTicker) const noexcept
	{
		return _asset == assetTicker || _priceUnit == assetTicker;
	}

	std::string tradable_pair::to_string(char separator) const
	{
		if (separator == '\0')
		{
			return to_string();
		}

		return _asset + separator + _priceUnit;
	}

	std::string tradable_pair::to_string() const
	{
		return _asset + _priceUnit;
	}

	bool tradable_pair::operator==(const tradable_pair& other) const noexcept
	{
		return _asset == other._asset && _priceUnit == other._priceUnit;
	}

	std::string_view get_gained_asset(const tradable_pair& pair, trade_action action)
	{
		return action == trade_action::BUY
			? pair.asset()
			: pair.price_unit();
	}

	tradable_pair parse_tradable_pair(std::string_view string, char seperator)
	{
		std::vector<std::string> assets{ split(string, seperator) };

		if (assets.size() != 2)
		{
			throw mb_exception{ fmt::format("Could not parse tradable pair from {0} with seperator \"{1}\"", string, seperator) };
		}

		to_upper(assets[0]);
		to_upper(assets[1]);

		return tradable_pair{ assets[0], assets[1] };
	}
}