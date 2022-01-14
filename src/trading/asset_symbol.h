#pragma once

#include <string>

namespace cb
{
	class asset_symbol
	{
	private:
		std::string _symbol;
		bool is_valid(const std::string& symbol);

	public:
		explicit asset_symbol(std::string symbol);

		const std::string& get() const { return _symbol; }

		bool operator==(const asset_symbol& other) const;
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