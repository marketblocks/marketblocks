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
		constexpr tradable_pair(asset_symbol asset, asset_symbol priceUnit)
			:
			_iso4217_a3{ asset.get() + "/" + priceUnit.get() },
			_exchangeId{ _iso4217_a3 },
			_asset{ std::move(asset) },
			_priceUnit{ std::move(priceUnit) }
		{}

		constexpr tradable_pair(std::string exchangeId, asset_symbol asset, asset_symbol priceUnit)
			:
			_iso4217_a3{ asset.get() + "/" + priceUnit.get() },
			_exchangeId{ std::move(exchangeId) },
			_asset{ std::move(asset) },
			_priceUnit{ std::move(priceUnit) }
		{}

		constexpr const std::string& iso_4217_a3() const noexcept { return _iso4217_a3; }
		constexpr const std::string& exchange_identifier() const noexcept { return _exchangeId; }
		constexpr const asset_symbol& asset() const noexcept { return _asset; }
		constexpr const asset_symbol& price_unit() const noexcept { return _priceUnit; }

		constexpr bool contains(const asset_symbol& assetTicker) const noexcept
		{
			return _asset == assetTicker || _priceUnit == assetTicker;
		}

		constexpr bool operator==(const tradable_pair& other) const noexcept
		{
			return _asset == other._asset && _priceUnit == other._priceUnit;
		}
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