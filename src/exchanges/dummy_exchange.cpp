#include "dummy_exchange.h"

double DummyExchange::get_fee() const
{
	return 0.1;
}

const std::vector<TradablePair> DummyExchange::get_tradable_pairs() const
{
	std::vector<TradablePair> pairs
	{
		TradablePair{ AssetSymbol::BTC, AssetSymbol::GBP },
		TradablePair{ AssetSymbol::BTC, AssetSymbol::USD },
		TradablePair{ AssetSymbol::ETH, AssetSymbol::GBP },
		TradablePair{ AssetSymbol::ETH, AssetSymbol::USD },
		TradablePair{ AssetSymbol::ETH, AssetSymbol::BTC },
		TradablePair{ AssetSymbol::LTC, AssetSymbol::USD },
		TradablePair{ AssetSymbol::LTC, AssetSymbol::BTC }
	};

	return pairs;
}

const std::unordered_map<TradablePair, PriceData> DummyExchange::get_price_data(const std::vector<TradablePair>& tradablePairs) const
{
	return std::unordered_map<TradablePair, PriceData>();
}
