#pragma once

#include <string>
#include <unordered_map>

enum class ExchangeId
{
	KRAKEN
};

class ExchangeIdLookup
{
private:
	std::unordered_map<std::string, ExchangeId> _map;

public:
	ExchangeIdLookup();

	const std::unordered_map<std::string, ExchangeId>& map() const { return _map; }
};