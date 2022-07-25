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
		tradable_pair(std::string asset, std::string priceUnit);

		const std::string& asset() const noexcept { return _asset; }
		const std::string& price_unit() const noexcept { return _priceUnit; }

		bool contains(std::string_view assetTicker) const noexcept;
		std::string to_string(char separator) const;
		std::string to_string() const;
		bool operator==(const tradable_pair& other) const noexcept;
	};

	std::string_view get_gained_asset(const tradable_pair& pair, trade_action action);

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
		std::size_t operator()(const mb::tradable_pair& pair) const
		{
			return std::hash<std::string>()(pair.asset()) ^ std::hash<std::string>()(pair.price_unit());
		}
	};
}