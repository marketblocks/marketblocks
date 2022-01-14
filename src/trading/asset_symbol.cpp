#include <stdexcept>
#include <format>

#include "asset_symbol.h"

namespace cb
{
	asset_symbol::asset_symbol(std::string symbol)
		: _symbol{ std::move(symbol) }
	{
		if (!is_valid(_symbol))
		{
			throw std::invalid_argument{ std::format("Symbol: '{}' is not a recognized AssetSymbol", _symbol) };
		}
	}

	bool asset_symbol::is_valid(const std::string& symbol)
	{
		return true;
	}

	bool asset_symbol::operator==(const asset_symbol& other) const
	{
		return _symbol == other._symbol;
	}
}