#pragma once

#include <cassert>
#include <string>
#include <string_view>

#include "trading_constants.h"

namespace cb
{
	class tradable_pair
	{
	private:
		std::string _asset;
		std::string _priceUnit;

	public:
		constexpr tradable_pair(std::string asset, std::string priceUnit)
			:
			_asset{ std::move(asset) },
			_priceUnit{ std::move(priceUnit) }
		{
			assert(!_asset.empty());
			assert(!_priceUnit.empty());
		}

		constexpr const std::string& asset() const noexcept { return _asset; }
		constexpr const std::string& price_unit() const noexcept { return _priceUnit; }

		constexpr bool contains(std::string_view assetTicker) const noexcept
		{
			return _asset == assetTicker || _priceUnit == assetTicker;
		}

		constexpr std::string to_standard_string() const
		{
			return _asset + "/" + _priceUnit;
		}

		constexpr bool operator==(const tradable_pair& other) const noexcept
		{
			return _asset == other._asset && _priceUnit == other._priceUnit;
		}
	};

	constexpr std::string_view get_gained_asset(const tradable_pair& pair, trade_action action)
	{
		return action == trade_action::BUY
			? pair.asset()
			: pair.price_unit();
	}
}

namespace std
{
	template<>
	struct hash<cb::tradable_pair>
	{
		size_t operator()(const cb::tradable_pair& pair) const
		{
			return std::hash<std::string>()(pair.asset()) ^ std::hash<std::string>()(pair.price_unit());
		}
	};
}