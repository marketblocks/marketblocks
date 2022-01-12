#include "tradable_pair.h"

TradablePair::TradablePair(AssetSymbol asset, AssetSymbol priceUnit)
	: 
	_iso4217_a3{ asset.get() + "/" + priceUnit.get() }, 
	_exchangeId{ _iso4217_a3 },
	_asset{ std::move(asset) }, 
	_priceUnit{ std::move(priceUnit) }
{}

TradablePair::TradablePair(std::string exchangeId, AssetSymbol asset, AssetSymbol priceUnit)
	: 
	_iso4217_a3{ asset.get() + "/" + priceUnit.get() }, 
	_exchangeId{ std::move(exchangeId) },
	_asset{ std::move(asset) }, 
	_priceUnit{ std::move(priceUnit) }
{}

bool TradablePair::contains(const AssetSymbol& assetTicker) const
{ 
	return _asset == assetTicker || _priceUnit == assetTicker; 
}

bool TradablePair::operator==(const TradablePair& other) const
{
	return _asset == other._asset && _priceUnit == other._priceUnit;
}