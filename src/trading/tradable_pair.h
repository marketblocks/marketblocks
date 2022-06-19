#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>

#include "trading_constants.h"

namespace mb
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
		{}

		constexpr const std::string& asset() const noexcept { return _asset; }
		constexpr const std::string& price_unit() const noexcept { return _priceUnit; }

		constexpr bool contains(std::string_view assetTicker) const noexcept
		{
			return _asset == assetTicker || _priceUnit == assetTicker;
		}

		constexpr std::string to_string(char separator) const
		{
			return _asset + separator + _priceUnit;
		}

		constexpr std::string to_string() const
		{
			return _asset + _priceUnit;
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

	tradable_pair parse_tradable_pair(std::string_view string, char seperator = '/');

	template<typename Container>
	std::vector<tradable_pair> parse_tradable_pairs(const Container& source, char seperator = '/')
	{
		std::vector<tradable_pair> pairs;
		pairs.reserve(source.size());

		for (auto& name : source)
		{
			pairs.emplace_back(parse_tradable_pair(name, seperator));
		}

		return pairs;
	}
}

namespace std
{
	template<>
	struct hash<mb::tradable_pair>
	{
		size_t operator()(const mb::tradable_pair& pair) const
		{
			return std::hash<std::string>()(pair.asset()) ^ std::hash<std::string>()(pair.price_unit());
		}
	};
}