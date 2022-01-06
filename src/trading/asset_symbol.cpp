#include <stdexcept>
#include <format>

#include "asset_symbol.h"

AssetSymbol::AssetSymbol(std::string symbol)
	: _symbol{ std::move(symbol) }
{
	if (!is_valid(_symbol))
	{
		throw std::invalid_argument{ std::format("Symbol: '{}' is not a recognized AssetSymbol", _symbol) };
	}
}

bool AssetSymbol::is_valid(const std::string& symbol)
{
	return true;
}

bool AssetSymbol::operator==(const AssetSymbol& other) const
{
	return _symbol == other._symbol;
}