#pragma once

#include <string>

namespace cb
{
	class asset_symbol
	{
	private:
		std::string _symbol;

	public:
		constexpr asset_symbol(std::string symbol)
			: _symbol{ std::move(symbol) }
		{}

		constexpr const std::string& get() const noexcept { return _symbol; }

		constexpr bool operator==(const asset_symbol& other) const noexcept
		{
			return _symbol == other._symbol;
		}
	};
}

namespace std
{
	template<>
	struct hash<cb::asset_symbol>
	{
		size_t operator()(const cb::asset_symbol& symbol) const
		{
			return std::hash<std::string>()(symbol.get());
		}
	};
}