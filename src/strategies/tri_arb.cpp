#include <iostream>
#include <algorithm>

#include "tri_arb.h"
#include "utils\vectorutils.h"
#include "utils\mathutils.h"
#include "utils\financeutils.h"

std::vector<TriArbExchangeSpec> create_exchange_specs(const std::vector<std::shared_ptr<Exchange>>& exchanges)
{
	std::vector<TriArbExchangeSpec> specs;
	specs.reserve(exchanges.size());

	for (std::shared_ptr<Exchange> exchange : exchanges)
	{
		const std::vector<TradablePair> tradablePairs = exchange->get_tradable_pairs();
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

					sequences.emplace_back(
						SequenceStep{ firstPair, TradeAction::BUY },
						SequenceStep{ middlePair, middleAction },
						SequenceStep{ finalPair, TradeAction::SELL },
						std::vector<TradablePair>{ firstPair, middlePair, finalPair });

					break;
				}
			}
		}

		specs.emplace_back(exchange, std::move(sequences));
	}

	return specs;
}

TriArbStrategy create_tri_arb_strategy(const std::vector<std::shared_ptr<Exchange>>& exchanges, TradingOptions options)
{
	std::vector<TriArbExchangeSpec> specs = create_exchange_specs(exchanges);
	return TriArbStrategy{ std::move(specs), std::move(options) };
}

static void print_pair(const TradablePair& pair)
{
	std::cout << pair.asset() << "/" << pair.price_unit();
}

static void print_sequence(const TriArbSequence& sequence)
{
	print_pair(sequence.first().pair());
	std::cout << "->";
	print_pair(sequence.middle().pair());
	std::cout << "->";
	print_pair(sequence.last().pair());
}

TriArbSequenceTrades TriArbStrategy::calculate_trades(const TriArbSequence& sequence, const std::unordered_map<TradablePair, PriceData>& prices, double initialTradeCost)
{
//	const SequenceStep& firstStep = sequence.first();
//	const SequenceStep& middleStep = sequence.middle();
//	const SequenceStep& lastStep = sequence.last();
//
//	const PriceData& firstPrices = prices.at(firstStep.pair());
//	const PriceData& middlePrices = prices.at(middleStep.pair());
//	const PriceData& lastPrices = prices.at(lastStep.pair());
//
//	TradeDescription firstTrade = create_trade_by_cost(firstStep.pair(), firstStep.action(), firstPrices, initialTradeCost);
//
//	TradeDescription middleTrade = middleStep.action() == TradeAction::BUY
//		? create_trade_by_cost(middleStep.pair(), TradeAction::BUY, middlePrices, firstTrade.volume())
//		: create_trade_by_volume(middleStep.pair(), TradeAction::SELL, middlePrices, firstTrade.volume());
//
//	TradeDescription lastTrade = lastStep.action() == TradeAction::BUY
//		? create_trade_by_cost(lastStep.pair(), TradeAction::BUY, lastPrices, )
//
	return TriArbSequenceTrades{ TradeDescription{TradablePair{}, TradeAction::BUY, 0, 0}, TradeDescription{TradablePair{}, TradeAction::BUY, 0, 0}, TradeDescription{TradablePair{}, TradeAction::BUY, 0, 0} };
}

void TriArbStrategy::run_iteration()
{
	for (auto& spec : _specs)
	{
		double tradeValue = _options.max_trade_percent() * spec.exchange().get_balance(AssetSymbol::GBP.data());

		for (auto& sequence : spec.sequences())
		{
			const std::unordered_map<TradablePair, PriceData> prices = spec.exchange().get_price_data(sequence.pairs());
			//TriArbSequenceTrades trades = calculate_trades(sequence, prices);

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

			double percentageDiff = calculate_percentage_diff(actualPrice, effectivePrice);
			double totalFee = spec.exchange().get_fee() * 3;

			if (percentageDiff > totalFee)
			{
				double tradeVolume = calculate_volume(firstPrices.ask(), tradeValue);
				spec.exchange().trade(TradeDescription{ sequence.first().pair(), sequence.first().action(), firstPrices.ask(), tradeVolume});

				double finalTradeVolume;
				if (sequence.middle().action() == TradeAction::BUY)
				{
					tradeVolume /= middlePrice;
					finalTradeVolume = tradeVolume;
				}
				else
				{
					finalTradeVolume = tradeVolume * middlePrice;
				}
					
				spec.exchange().trade(TradeDescription{ sequence.middle().pair(), sequence.middle().action(), middlePrice, tradeVolume });
				spec.exchange().trade(TradeDescription{ sequence.last().pair(), sequence.last().action(), lastPrices.bid(), finalTradeVolume });

				std::cout << "TRADE:" << std::endl;
				std::cout << "Sequence: ";
				print_sequence(sequence);
				std::cout << std::endl;

				std::cout << "Percentage Difference: " << percentageDiff << "%" << std::endl;
				std::cout << "Total Fee: " << totalFee << "%" << std::endl;
				std::cout << "Potential Profit: " << percentageDiff - totalFee << "%" << std::endl;

				std::cout << "New GBP Balance: " << spec.exchange().get_balance("GBP") << std::endl;
			}
		}
	}
}
