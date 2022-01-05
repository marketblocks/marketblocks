#pragma once

#include <string>

namespace exchange_identifiers
{
	static const std::string Kraken = "kraken";
}

enum class ExchangeId
{
	KRAKEN
};

class ExchangeIdLookup
{
private:
	std::unordered_map<std::string, ExchangeId> _map;

public:
	ExchangeIdLookup()
		: _map
	{
		{ exchange_identifiers::Kraken, ExchangeId::KRAKEN }
	}
	{}

	const std::unordered_map<std::string, ExchangeId>& map() const { return _map; }
};