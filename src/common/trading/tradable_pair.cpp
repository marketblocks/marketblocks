#include "tradable_pair.h"

TradablePair::TradablePair()
	:_exchangeId{}, _asset{}, _priceUnit{}
{}

TradablePair::TradablePair(std::string asset, std::string priceUnit)
	: _exchangeId{ asset + priceUnit }, _asset{ std::move(asset) }, _priceUnit{ std::move(priceUnit) }
{}

TradablePair::TradablePair(std::string exchangeId, std::string asset, std::string priceUnit)
	: _exchangeId{ std::move(exchangeId) }, _asset{ std::move(asset) }, _priceUnit{ std::move(priceUnit) }
{}

bool TradablePair::contains(const std::string& assetTicker) const 
{ 
	return _asset == assetTicker || _priceUnit == assetTicker; 
}

bool TradablePair::operator==(const TradablePair& other) const
{
	return _asset == other._asset && _priceUnit == other._priceUnit;
}