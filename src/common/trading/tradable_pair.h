#pragma once

#include <string>
#include <string_view>

#include "asset_symbol.h"

class TradablePair
{
private:
	std::string _exchangeId;
	AssetSymbol _asset;
	AssetSymbol _priceUnit;

public:
	explicit TradablePair(AssetSymbol asset, AssetSymbol priceUnit);
	explicit TradablePair(std::string exchangeId, AssetSymbol asset, AssetSymbol priceUnit);

	const std::string& exchange_identifier() const { return _exchangeId; }
	const AssetSymbol& asset() const { return _asset; }
	const AssetSymbol& price_unit() const { return _priceUnit; }

	bool contains(const AssetSymbol& assetTicker) const;

	bool operator==(const TradablePair& other) const;
};

namespace std
{
	template<>
	struct hash<TradablePair>
	{
		size_t operator()(const TradablePair& pair) const
		{
			return std::hash<AssetSymbol>()(pair.asset()) ^ std::hash<AssetSymbol>()(pair.price_unit());
		}
	};
}