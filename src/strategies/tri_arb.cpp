#include <iostream>
#include <algorithm>

#include "tri_arb.h"
#include "..\utils\vectorutils.h"

TriArbStrategy TriArbStrategy::create(const std::vector<std::shared_ptr<Exchange>>& exchanges)
{
	std::vector<TriArbExchangeSpec> specs;
	specs.reserve(exchanges.size());

	for (auto& exchange : exchanges)
	{
		const std::vector<TradablePair> tradablePairs = exchange->get_tradable_pairs();
		std::vector<TriArbSequence> sequences;
		std::vector<TradablePair> gbpTradables = copy_where(tradablePairs, [](const TradablePair& pair) { return pair.price_unit() == AssetSymbol::GBP; });
		
		for (auto& firstPair : gbpTradables)
		{
			AssetSymbol purchasedAsset = firstPair.asset();
			
			std::vector<TradablePair> possibleMiddles = copy_where(tradablePairs, [purchasedAsset](const TradablePair& pair)
			{ 
				return (pair.asset() == purchasedAsset && pair.price_unit() != AssetSymbol::GBP) || pair.price_unit() == purchasedAsset;
			});

			for (auto& middlePair : possibleMiddles)
			{
				AssetSymbol middleOtherAsset;
				TradeAction middleAction;

				if (middlePair.asset() == purchasedAsset)
				{
					middleOtherAsset = middlePair.price_unit();
					middleAction = TradeAction::SELL;
				}
				else
				{
					middleOtherAsset = middlePair.asset();
					middleAction = TradeAction::BUY;
				}

				auto iterator = std::find_if(tradablePairs.begin(), tradablePairs.end(), [middleOtherAsset](const TradablePair& pair) { return pair.asset() == middleOtherAsset && pair.price_unit() == AssetSymbol::GBP; });

				if (iterator != tradablePairs.end())
				{
					const TradablePair& finalPair = *iterator;

					TradeDescription firstTrade{ firstPair, TradeAction::BUY };
					TradeDescription middleTrade{ middlePair, middleAction };
					TradeDescription finalTrade{ finalPair, TradeAction::SELL };
					std::vector<TradablePair> pairs{ firstPair, middlePair, finalPair };

					sequences.emplace_back(firstTrade, middleTrade, finalTrade, std::move(pairs));

					break;
				}
			}
		}

		specs.emplace_back(exchange, std::move(sequences));
	}

	TriArbStrategy s{ std::move(specs) };
	return s;
}

void TriArbStrategy::operator()()
{
	for (auto& seq : _specs[0].sequences())
	{
		std::cout << "Sequence: " << static_cast<int>(seq.first().pair().asset()) << std::endl;

	}
}
