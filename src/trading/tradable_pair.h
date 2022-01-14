#pragma once

#include <string>
#include <string_view>

#include "asset_symbol.h"

namespace cb
{
	class tradable_pair
	{
	private:
		std::string _iso4217_a3;
		std::string _exchangeId;
		asset_symbol _asset;
		asset_symbol _priceUnit;

	public:
		explicit tradable_pair(asset_symbol asset, asset_symbol priceUnit);
		explicit tradable_pair(std::string exchangeId, asset_symbol asset, asset_symbol priceUnit);

		const std::string& iso_4217_a3() const { return _iso4217_a3; }
		const std::string& exchange_identifier() const { return _exchangeId; }
		const asset_symbol& asset() const { return _asset; }
		const asset_symbol& price_unit() const { return _priceUnit; }

		bool contains(const asset_symbol& assetTicker) const;

		bool operator==(const tradable_pair& other) const;
	};
}

namespace std
{
	template<>
	struct hash<cb::tradable_pair>
	{
		size_t operator()(const cb::tradable_pair& pair) const
		{
			return std::hash<cb::asset_symbol>()(pair.asset()) ^ std::hash<cb::asset_symbol>()(pair.price_unit());
		}
	};
}