#include <iostream>
#include <algorithm>

#include "tri_arb.h"
#include "..\utils\vectorutils.h"

TriArbStrategy TriArbStrategy::create(std::vector<Exchange>& exchanges)
{
	std::vector<TriArbExchangeSpec> specs;
	specs.reserve(exchanges.size());

	for (auto& exchange : exchanges)
	{
		const std::vector<TradablePair> tradablePairs = exchange.get_tradable_pairs();
		std::vector<TriArbSequence> sequences;
		std::vector<TradablePair> gbpTradables = copy_where(tradablePairs, [](const TradablePair& pair) { return pair.price_unit() == AssetSymbol::GBP; });
		
		for (auto& firstPair : gbpTradables)
		{
			const std::string_view& purchasedAsset = firstPair.asset();
			
			std::vector<TradablePair> possibleMiddles = copy_where(tradablePairs, [purchasedAsset](const TradablePair& pair)
			{ 
				return (pair.asset() == purchasedAsset && pair.price_unit() != AssetSymbol::GBP) || pair.price_unit() == purchasedAsset;
			});

			for (auto& middlePair : possibleMiddles)
			{
				std::string_view middleOtherAsset;
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

void print_pair(const TradablePair& pair)
{
	std::cout << pair.asset() << "/" << pair.price_unit();
}

void print_sequence(const TriArbSequence& sequence)
{
	print_pair(sequence.first().pair());
	std::cout << "->";
	print_pair(sequence.middle().pair());
	std::cout << "->";
	print_pair(sequence.last().pair());
}

void TriArbStrategy::operator()()
{
	for (auto& spec : _specs)
	{
		for (auto& sequence : spec.sequences())
		{
			const std::unordered_map<TradablePair, PriceData> prices = spec.exchange().get_price_data(sequence.pairs());

			PriceData firstPrices = prices.at(sequence.first().pair());
			PriceData middlePrices = prices.at(sequence.middle().pair());
			PriceData lastPrices = prices.at(sequence.last().pair());

			double effectivePrice;
			double actualPrice;
			double middlePrice;

			if (sequence.middle().action() == TradeAction::BUY)
			{
				middlePrice = middlePrices.ask();
				effectivePrice = lastPrices.bid() / firstPrices.ask();
				actualPrice = middlePrice;
			}
			else
			{
				middlePrice = middlePrices.bid();
				effectivePrice = middlePrice * lastPrices.bid();
				actualPrice = firstPrices.ask();
			}

			double percentageDiff = (effectivePrice - actualPrice) * 100 / actualPrice;
			double totalFee = spec.exchange().get_fee() * 3;

			if (percentageDiff > 1e-4)
			{
				std::cout << "Sequence: ";
				print_sequence(sequence);
				std::cout << std::endl;

				std::cout << "Percentage Difference: " << percentageDiff << "%" << std::endl;
				std::cout << "Total Fee: " << totalFee << "%" << std::endl;
				std::cout << "Potential Profit: " << percentageDiff - totalFee << "%" << std::endl;

				if (percentageDiff - totalFee > 1)
				{
					std::cout << "profit";
				}
			}

			//if (percentageDiff > totalFee)
			//{
			//	// trade
			//}
		}
	}
}
