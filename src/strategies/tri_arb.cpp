#include <iostream>
#include <algorithm>

#include "tri_arb.h"
#include "utils\vectorutils.h"
#include "utils\mathutils.h"
#include "utils\financeutils.h"

std::vector<TriArbExchangeSpec> create_exchange_specs(const std::vector<std::shared_ptr<Exchange>>& exchanges, const std::string& fiatCurrency)
{
	std::vector<TriArbExchangeSpec> specs;
	specs.reserve(exchanges.size());

	for (std::shared_ptr<Exchange> exchange : exchanges)
	{
		const std::vector<TradablePair> tradablePairs = exchange->get_tradable_pairs();
		std::vector<TriArbSequence> sequences;

		std::vector<TradablePair> fiatTradables = copy_where(
			tradablePairs, 
			[&fiatCurrency](const TradablePair& pair) { return pair.contains(fiatCurrency); });

		std::vector<TradablePair> nonFiatTradables = copy_where(
			tradablePairs,
			[&fiatTradables](const TradablePair& pair) { return !contains(fiatTradables, pair); });

		for (auto& firstPair : fiatTradables)
		{
			TradeAction firstAction;
			std::string firstGainedAsset;

			if (firstPair.price_unit() == fiatCurrency)
			{
				firstAction = TradeAction::BUY;
				firstGainedAsset = firstPair.asset();
			}
			else
			{
				firstAction = TradeAction::SELL;
				firstGainedAsset = firstPair.price_unit();
			}

			std::vector<TradablePair> possibleMiddles = copy_where(
				nonFiatTradables,
				[&firstGainedAsset](const TradablePair& pair) { return pair.contains(firstGainedAsset); });

			for (auto& middlePair : possibleMiddles)
			{
				TradeAction middleAction;
				std::string middleGainedAsset;

				if (middlePair.price_unit() == firstGainedAsset)
				{
					middleAction = TradeAction::BUY;
					middleGainedAsset = middlePair.asset();
				}
				else
				{
					middleAction = TradeAction::SELL;
					middleGainedAsset = middlePair.price_unit();
				}

				std::vector<TradablePair> finals = copy_where(
					fiatTradables,
					[&middleGainedAsset](const TradablePair& pair) { return pair.contains(middleGainedAsset); });

				for (auto& finalPair : finals)
				{
					TradeAction finalAction = finalPair.price_unit() == middleGainedAsset
						? TradeAction::BUY
						: TradeAction::SELL;

					sequences.emplace_back(
						SequenceStep{ firstPair, firstAction },
						SequenceStep{ middlePair, middleAction },
						SequenceStep{ finalPair, finalAction },
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
	std::vector<TriArbExchangeSpec> specs = create_exchange_specs(exchanges, options.fiat_currency());
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

static void log_trade(const TriArbSequence& sequence, double percentageProfit, double newBalance)
{
	std::cout << "TRADE:" << std::endl;
	std::cout << "Sequence: ";
	print_sequence(sequence);
	std::cout << std::endl;

	std::cout << "Profit: " << percentageProfit << "%" << std::endl;
	std::cout << "New Balance: " << newBalance << std::endl;
}

TriArbSequenceTradeStep TriArbStrategy::create_sequence_trade_step(const SequenceStep& sequenceStep, const PriceData& priceData, double fee, double previousTradeGain)
{
	if (sequenceStep.action() == TradeAction::BUY)
	{
		double cost = previousTradeGain - calculate_fee(previousTradeGain, fee);
		TradeDescription description = create_trade_by_cost(sequenceStep.pair(), TradeAction::BUY, priceData, cost);
		return TriArbSequenceTradeStep{ std::move(description), description.volume() };
	}
	else
	{
		TradeDescription description = create_trade_by_volume(sequenceStep.pair(), TradeAction::SELL, priceData, previousTradeGain);
		double newTradeGain = calculate_cost(description.asset_price(), description.volume());
		newTradeGain -= calculate_fee(newTradeGain, fee);
		return TriArbSequenceTradeStep{ std::move(description), newTradeGain };
	}
}

TriArbSequenceTrades TriArbStrategy::calculate_trades(const TriArbSequence& sequence, const std::unordered_map<TradablePair, PriceData>& prices, const std::unordered_map<TradablePair, double>& fees, double initialTradeValue)
{
	const SequenceStep& firstStep = sequence.first();
	const SequenceStep& middleStep = sequence.middle();
	const SequenceStep& lastStep = sequence.last();

	const PriceData& firstPrices = prices.at(firstStep.pair());
	const PriceData& middlePrices = prices.at(middleStep.pair());
	const PriceData& lastPrices = prices.at(lastStep.pair());

	double firstFee = fees.at(firstStep.pair());
	double middleFee = fees.at(middleStep.pair());
	double lastFee = fees.at(lastStep.pair());
		
	TriArbSequenceTradeStep firstTrade = create_sequence_trade_step(firstStep, firstPrices, firstFee, initialTradeValue);
	TriArbSequenceTradeStep middleTrade = create_sequence_trade_step(middleStep, middlePrices, middleFee, firstTrade.gain_value());
	TriArbSequenceTradeStep lastTrade = create_sequence_trade_step(lastStep, lastPrices, lastFee, middleTrade.gain_value());
	
	return TriArbSequenceTrades{ firstTrade.description(), middleTrade.description(), lastTrade.description() };
}

double TriArbStrategy::calculate_profit(const TriArbSequenceTrades& trades)
{
	double beforeBalance = simulator.get_balance(_options.fiat_currency());

	simulator.trade(trades.first());
	simulator.trade(trades.middle());
	simulator.trade(trades.last());

	double afterBalance = simulator.get_balance(_options.fiat_currency());

	return calculate_percentage_diff(beforeBalance, afterBalance);
}

void TriArbStrategy::run_iteration()
{
	for (auto& spec : _specs)
	{
		double tradeValue = _options.max_trade_percent() * spec.exchange().get_balance(_options.fiat_currency());
		Exchange& exchange = spec.exchange();

		for (auto& sequence : spec.sequences())
		{
			const std::unordered_map<TradablePair, PriceData> prices = exchange.get_price_data(sequence.pairs());
			const std::unordered_map<TradablePair, double> fees = exchange.get_fees(sequence.pairs());

			TriArbSequenceTrades trades = calculate_trades(sequence, prices, fees, tradeValue);
			double percentageProfit = calculate_profit(trades);

			if (percentageProfit > 0)
			{
				exchange.trade(trades.first());
				exchange.trade(trades.middle());
				exchange.trade(trades.last());
				
				log_trade(sequence, percentageProfit, spec.exchange().get_balance(_options.fiat_currency()));
			}
		}
	}
}
