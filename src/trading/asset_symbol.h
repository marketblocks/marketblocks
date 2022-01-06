#pragma once

#include <string>

class AssetSymbol
{
private:
	std::string _symbol;
	bool is_valid(const std::string& symbol);

public:
	explicit AssetSymbol(std::string symbol);

	const std::string& get() const { return _symbol; }

	bool operator==(const AssetSymbol& other) const;
};

namespace std
{
	template<>
	struct hash<AssetSymbol>
	{
		size_t operator()(const AssetSymbol& symbol) const
		{
			return std::hash<std::string>()(symbol.get());
		}
	};
}