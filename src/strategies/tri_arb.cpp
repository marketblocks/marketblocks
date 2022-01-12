#include <iostream>
#include <algorithm>
#include <unordered_set>

#include "tri_arb.h"
#include "trading/order_book.h"
#include "trading/trade_description.h"
#include "exchanges/exchange_helpers.h"
#include "common/utils/vectorutils.h"
#include "common/utils/mathutils.h"
#include "common/utils/financeutils.h"

SequenceStep::SequenceStep(TradablePair pair, TradeAction action)
	: _pair{ std::move(pair) }, _action{ std::move(action) }
{}

bool SequenceStep::operator==(const SequenceStep& other) const
{
	return _action == other._action && _pair == other._pair;
}

TriArbSequence::TriArbSequence(SequenceStep first, SequenceStep middle, SequenceStep last, std::vector<TradablePair> pairs)
	: _first{ std::move(first) }, _middle{ std::move(middle) }, _last{ std::move(last) }, _pairs{ std::move(pairs) }
{}

TriArbExchangeSpec::TriArbExchangeSpec(std::shared_ptr<Exchange> exchange, std::vector<TriArbSequence> sequences)
	: _exchange{ exchange }, _sequences{ std::move(sequences) }
{}

std::vector<TriArbExchangeSpec> create_exchange_specs(const std::vector<std::shared_ptr<Exchange>>& exchanges, const AssetSymbol& fiatCurrency)
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
			std::unique_ptr<AssetSymbol> firstGainedAsset;

			if (firstPair.price_unit() == fiatCurrency)
			{
				firstAction = TradeAction::BUY;
				firstGainedAsset = std::make_unique<AssetSymbol>(firstPair.asset());
			}
			else
			{
				firstAction = TradeAction::SELL;
				firstGainedAsset = std::make_unique<AssetSymbol>(firstPair.price_unit());
			}

			std::vector<TradablePair> possibleMiddles = copy_where(
				nonFiatTradables,
				[&firstGainedAsset](const TradablePair& pair) { return pair.contains(*firstGainedAsset); });

			for (auto& middlePair : possibleMiddles)
			{
				TradeAction middleAction;
				std::unique_ptr<AssetSymbol> middleGainedAsset;

				if (middlePair.price_unit() == *firstGainedAsset)
				{
					middleAction = TradeAction::BUY;
					middleGainedAsset = std::make_unique<AssetSymbol>(middlePair.asset());
				}
				else
				{
					middleAction = TradeAction::SELL;
					middleGainedAsset = std::make_unique<AssetSymbol>(middlePair.price_unit());
				}

				std::vector<TradablePair> finals = copy_where(
					fiatTradables,
					[&middleGainedAsset](const TradablePair& pair) { return pair.contains(*middleGainedAsset); });

				for (auto& finalPair : finals)
				{
					TradeAction finalAction = finalPair.price_unit() == *middleGainedAsset
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

namespace
{
	struct SequenceTrades
	{
		TradeDescription first;
		TradeDescription middle;
		TradeDescription last;
	};

	struct SequenceTradeStep
	{
		TradeDescription description;
		double gainValue;
	};

	struct SequenceGains
	{
		double g1;
		double g2;
		double g3;
	};

	void print_pair(const TradablePair& pair)
	{
		std::cout << pair.asset().get() << "/" << pair.price_unit().get();
	}

	void print_sequence(const TriArbSequence& sequence)
	{
		std::cout << "Sequence: ";

		print_pair(sequence.first().pair());
		std::cout << "->";
		print_pair(sequence.middle().pair());
		std::cout << "->";
		print_pair(sequence.last().pair());

		std::cout << std::endl;
	}

	void print_trade(const TradeDescription& tradeDescription, double gain)
	{
		char action;
		double result;

		if (tradeDescription.action() == TradeAction::BUY)
		{
			action = 'B';
			result = calculate_cost(tradeDescription.asset_price(), tradeDescription.volume());
		}
		else
		{
			action = 'S';
			result = gain;
		}

		std::cout << action << " " << tradeDescription.volume() << " ";
		print_pair(tradeDescription.pair());
		std::cout << " @ " << tradeDescription.asset_price() << " = " << result << std::endl;
	}

	void log_trade(const TriArbSequence& sequence, const SequenceTrades& trades, const SequenceGains& gains, double initialTradeValue, double newBalance)
	{
		std::cout << std::endl << "TRADE:" << std::endl;

		print_sequence(sequence);

		print_trade(trades.first, gains.g1);
		print_trade(trades.middle, gains.g2);
		print_trade(trades.last, gains.g3);

		double percentageProfit = calculate_percentage_diff(initialTradeValue, gains.g3);
		std::cout << "Profit: " << percentageProfit << "%" << std::endl;

		std::cout << "New Balance: " << newBalance << std::endl << std::endl;
	}

	void log_sequence_checked(const TriArbSequence& sequence, double initialTradeValue, double g3)
	{
		print_sequence(sequence);

		double percentageDiff = calculate_percentage_diff(initialTradeValue, g3);
		std::cout << "Price Diff: " << percentageDiff << "%" << std::endl;
	}

	TradeDescription create_trade(const SequenceStep& sequenceStep, const std::unordered_map<TradablePair, OrderBookLevel>& prices, double g1, double g0)
	{
		double volume = sequenceStep.action() == TradeAction::BUY ? g1 : g0;
		double assetPrice = select_entry(prices.at(sequenceStep.pair()), sequenceStep.action()).price();

		return TradeDescription(sequenceStep.pair(), sequenceStep.action(), assetPrice, volume);
	}

	SequenceTrades create_sequence_trades(const TriArbSequence& sequence, const SequenceGains& gains, const std::unordered_map<TradablePair, OrderBookLevel>& prices, double initialTradeValue)
	{
		TradeDescription firstTrade = create_trade(sequence.first(), prices, gains.g1, initialTradeValue);
		TradeDescription middleTrade = create_trade(sequence.middle(), prices, gains.g2, gains.g1);
		TradeDescription lastTrade = create_trade(sequence.last(), prices, gains.g3, gains.g2);

		return SequenceTrades{ std::move(firstTrade), std::move(middleTrade), std::move(lastTrade) };
	}

	SequenceGains calculate_sequence_gains(const TriArbSequence& sequence, const std::unordered_map<TradablePair, OrderBookLevel>& prices, const std::unordered_map<TradablePair, double>& fees, double initialTradeValue)
	{
		const SequenceStep& firstStep = sequence.first();
		const SequenceStep& middleStep = sequence.middle();
		const SequenceStep& lastStep = sequence.last();

		const OrderBookEntry& firstOrderBookEntry = select_entry(prices.at(firstStep.pair()), firstStep.action());
		const OrderBookEntry& middleOrderBookEntry = select_entry(prices.at(middleStep.pair()), middleStep.action());
		const OrderBookEntry& lastOrderBookEntry = select_entry(prices.at(lastStep.pair()), lastStep.action());

		double firstFee = fees.at(firstStep.pair());
		double middleFee = fees.at(middleStep.pair());
		double lastFee = fees.at(lastStep.pair());

		double g1 = calculate_trade_gain(firstOrderBookEntry.price(), initialTradeValue, firstFee, firstStep.action());
		double g2 = calculate_trade_gain(middleOrderBookEntry.price(), g1, middleFee, middleStep.action());
		double g3 = calculate_trade_gain(lastOrderBookEntry.price(), g2, lastFee, lastStep.action());

		return SequenceGains{ g1, g2, g3 };
	}
}

#include <chrono>
#include <thread>

void TriArbStrategy::initialise(const StrategyInitialiser& initaliser)
{
	_options = initaliser.options();
	_specs = create_exchange_specs(initaliser.exchanges(), _options.fiat_currency());


	for (auto& spec : _specs)
	{
		std::unordered_set<TradablePair> allPairs;

		for (auto& sequence : spec.sequences())
		{
			allPairs.insert(sequence.pairs().begin(), sequence.pairs().end());
		}

		spec.exchange().get_or_connect_websocket().subscribe_order_book(std::vector<TradablePair>{ allPairs.begin(), allPairs.end() });
	}

	std::this_thread::sleep_for(std::chrono::seconds(5));
}

void TriArbStrategy::run_iteration()
{
	for (auto& spec : _specs)
	{
		Exchange& exchange = spec.exchange();
		double tradeValue = _options.max_trade_percent() * get_balance(exchange, _options.fiat_currency());

		for (auto& sequence : spec.sequences())
		{
			const std::unordered_map<TradablePair, OrderBookLevel> prices = get_best_order_book_prices(exchange.get_or_connect_websocket(), sequence.pairs());
			const std::unordered_map<TradablePair, double> fees = exchange.get_fees(sequence.pairs());

			SequenceGains gains = calculate_sequence_gains(sequence, prices, fees, tradeValue);

			if (gains.g3 > tradeValue)
			{
				SequenceTrades trades = create_sequence_trades(sequence, gains, prices, tradeValue);

				exchange.trade(trades.first);
				exchange.trade(trades.middle);
				exchange.trade(trades.last);

				log_trade(sequence, trades, gains, tradeValue, get_balance(exchange, _options.fiat_currency()));
			}
			else
			{
				log_sequence_checked(sequence, tradeValue, gains.g3);
			}
		}
	}
}