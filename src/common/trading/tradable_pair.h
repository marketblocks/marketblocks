#pragma once

#include <string>
#include <string_view>

class TradablePair
{
private:
	std::string _exchangeId;
	std::string _asset;
	std::string _priceUnit;

public:
	explicit TradablePair();
	explicit TradablePair(std::string asset, std::string priceUnit);
	explicit TradablePair(std::string exchangeId, std::string asset, std::string priceUnit);

	const std::string& exchange_identifier() const { return _exchangeId; }
	const std::string& asset() const { return _asset; }
	const std::string& price_unit() const { return _priceUnit; }

	bool contains(const std::string& assetTicker) const;

	bool operator==(const TradablePair& other) const;
};

namespace std
{
	template<>
	struct hash<TradablePair>
	{
		size_t operator()(const TradablePair& pair) const
		{
			return std::hash<std::string_view>()(pair.asset()) ^ std::hash<std::string_view>()(pair.price_unit());
		}
	};
}