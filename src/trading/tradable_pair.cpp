#include "tradable_pair.h"

namespace cb
{
	tradable_pair::tradable_pair(asset_symbol asset, asset_symbol priceUnit)
		:
		_iso4217_a3{ asset.get() + "/" + priceUnit.get() },
		_exchangeId{ _iso4217_a3 },
		_asset{ std::move(asset) },
		_priceUnit{ std::move(priceUnit) }
	{}

	tradable_pair::tradable_pair(std::string exchangeId, asset_symbol asset, asset_symbol priceUnit)
		:
		_iso4217_a3{ asset.get() + "/" + priceUnit.get() },
		_exchangeId{ std::move(exchangeId) },
		_asset{ std::move(asset) },
		_priceUnit{ std::move(priceUnit) }
	{}

	bool tradable_pair::contains(const asset_symbol& assetTicker) const
	{
		return _asset == assetTicker || _priceUnit == assetTicker;
	}

	bool tradable_pair::operator==(const tradable_pair& other) const
	{
		return _asset == other._asset && _priceUnit == other._priceUnit;
	}
}