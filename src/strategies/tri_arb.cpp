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

sequence_step::sequence_step(cb::tradable_pair pair, cb::trade_action action)
	: _pair{ std::move(pair) }, _action{ std::move(action) }
{}

bool sequence_step::operator==(const sequence_step& other) const
{
	return _action == other._action && _pair == other._pair;
}

tri_arb_sequence::tri_arb_sequence(sequence_step first, sequence_step middle, sequence_step last, std::vector<cb::tradable_pair> pairs)
	: _first{ std::move(first) }, _middle{ std::move(middle) }, _last{ std::move(last) }, _pairs{ std::move(pairs) }
{}

tri_arb_exchange_spec::tri_arb_exchange_spec(std::shared_ptr<cb::exchange> exchange, std::vector<tri_arb_sequence> sequences)
	: _exchange{ exchange }, _sequences{ std::move(sequences) }
{}

std::vector<tri_arb_exchange_spec> create_exchange_specs(const std::vector<std::shared_ptr<cb::exchange>>& exchanges, const cb::asset_symbol& fiatCurrency)
{
	std::vector<tri_arb_exchange_spec> specs;
	specs.reserve(exchanges.size());

	for (std::shared_ptr<cb::exchange> exchange : exchanges)
	{
		const std::vector<cb::tradable_pair> tradablePairs = exchange->get_tradable_pairs();
		std::vector<tri_arb_sequence> sequences;

		std::vector<cb::tradable_pair> fiatTradables = copy_where(
			tradablePairs, 
			[&fiatCurrency](const cb::tradable_pair& pair) { return pair.contains(fiatCurrency); });

		std::vector<cb::tradable_pair> nonFiatTradables = copy_where(
			tradablePairs,
			[&fiatTradables](const cb::tradable_pair& pair) { return !contains(fiatTradables, pair); });

		for (auto& firstPair : fiatTradables)
		{
			cb::trade_action firstAction;
			std::unique_ptr<cb::asset_symbol> firstGainedAsset;

			if (firstPair.price_unit() == fiatCurrency)
			{
				firstAction = cb::trade_action::BUY;
				firstGainedAsset = std::make_unique<cb::asset_symbol>(firstPair.asset());
			}
			else
			{
				firstAction = cb::trade_action::SELL;
				firstGainedAsset = std::make_unique<cb::asset_symbol>(firstPair.price_unit());
			}

			std::vector<cb::tradable_pair> possibleMiddles = copy_where(
				nonFiatTradables,
				[&firstGainedAsset](const cb::tradable_pair& pair) { return pair.contains(*firstGainedAsset); });

			for (auto& middlePair : possibleMiddles)
			{
				cb::trade_action middleAction;
				std::unique_ptr<cb::asset_symbol> middleGainedAsset;

				if (middlePair.price_unit() == *firstGainedAsset)
				{
					middleAction = cb::trade_action::BUY;
					middleGainedAsset = std::make_unique<cb::asset_symbol>(middlePair.asset());
				}
				else
				{
					middleAction = cb::trade_action::SELL;
					middleGainedAsset = std::make_unique<cb::asset_symbol>(middlePair.price_unit());
				}

				std::vector<cb::tradable_pair> finals = copy_where(
					fiatTradables,
					[&middleGainedAsset](const cb::tradable_pair& pair) { return pair.contains(*middleGainedAsset); });

				for (auto& finalPair : finals)
				{
					cb::trade_action finalAction = finalPair.price_unit() == *middleGainedAsset
						? cb::trade_action::BUY
						: cb::trade_action::SELL;

					sequences.emplace_back(
						sequence_step{ firstPair, firstAction },
						sequence_step{ middlePair, middleAction },
						sequence_step{ finalPair, finalAction },
						std::vector<cb::tradable_pair>{ firstPair, middlePair, finalPair });

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
		cb::trade_description first;
		cb::trade_description middle;
		cb::trade_description last;
	};

	struct SequenceTradeStep
	{
		cb::trade_description description;
		double gainValue;
	};

	struct SequenceGains
	{
		double g1;
		double g2;
		double g3;
	};

	void print_pair(const cb::tradable_pair& pair)
	{
		std::cout << pair.asset().get() << "/" << pair.price_unit().get();
	}

	void print_sequence(const tri_arb_sequence& sequence)
	{
		std::cout << "Sequence: ";

		print_pair(sequence.first().pair());
		std::cout << "->";
		print_pair(sequence.middle().pair());
		std::cout << "->";
		print_pair(sequence.last().pair());

		std::cout << std::endl;
	}

	void print_trade(const cb::trade_description& tradeDescription, double gain)
	{
		char action;
		double result;

		if (tradeDescription.action() == cb::trade_action::BUY)
		{
			action = 'B';
			result = cb::calculate_cost(tradeDescription.asset_price(), tradeDescription.volume());
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

	void log_trade(const tri_arb_sequence& sequence, const SequenceTrades& trades, const SequenceGains& gains, double initialTradeValue, double newBalance)
	{
		std::cout << std::endl << "TRADE:" << std::endl;

		print_sequence(sequence);

		print_trade(trades.first, gains.g1);
		print_trade(trades.middle, gains.g2);
		print_trade(trades.last, gains.g3);

		double percentageProfit = cb::calculate_percentage_diff(initialTradeValue, gains.g3);
		std::cout << "Profit: " << percentageProfit << "%" << std::endl;

		std::cout << "New Balance: " << newBalance << std::endl << std::endl;
	}

	void log_sequence_checked(const tri_arb_sequence& sequence, double initialTradeValue, double g3)
	{
		print_sequence(sequence);

		double percentageDiff = cb::calculate_percentage_diff(initialTradeValue, g3);
		std::cout << "Price Diff: " << percentageDiff << "%" << std::endl;
	}

	cb::trade_description create_trade(const sequence_step& sequenceStep, const std::unordered_map<cb::tradable_pair, cb::order_book_level>& prices, double g1, double g0)
	{
		double volume = sequenceStep.action() == cb::trade_action::BUY ? g1 : g0;
		double assetPrice = select_entry(prices.at(sequenceStep.pair()), sequenceStep.action()).price();

		return cb::trade_description(sequenceStep.pair(), sequenceStep.action(), assetPrice, volume);
	}

	SequenceTrades create_sequence_trades(const tri_arb_sequence& sequence, const SequenceGains& gains, const std::unordered_map<cb::tradable_pair, cb::order_book_level>& prices, double initialTradeValue)
	{
		cb::trade_description firstTrade = create_trade(sequence.first(), prices, gains.g1, initialTradeValue);
		cb::trade_description middleTrade = create_trade(sequence.middle(), prices, gains.g2, gains.g1);
		cb::trade_description lastTrade = create_trade(sequence.last(), prices, gains.g3, gains.g2);

		return SequenceTrades{ std::move(firstTrade), std::move(middleTrade), std::move(lastTrade) };
	}

	SequenceGains calculate_sequence_gains(const tri_arb_sequence& sequence, const std::unordered_map<cb::tradable_pair, cb::order_book_level>& prices, const std::unordered_map<cb::tradable_pair, double>& fees, double initialTradeValue)
	{
		const sequence_step& firstStep = sequence.first();
		const sequence_step& middleStep = sequence.middle();
		const sequence_step& lastStep = sequence.last();

		const cb::order_book_entry& firstOrderBookEntry = select_entry(prices.at(firstStep.pair()), firstStep.action());
		const cb::order_book_entry& middleOrderBookEntry = select_entry(prices.at(middleStep.pair()), middleStep.action());
		const cb::order_book_entry& lastOrderBookEntry = select_entry(prices.at(lastStep.pair()), lastStep.action());

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

void tri_arb_strategy::initialise(const cb::strategy_initialiser& initaliser)
{
	_options = initaliser.options();
	_specs = create_exchange_specs(initaliser.exchanges(), _options.fiat_currency());


	for (auto& spec : _specs)
	{
		std::unordered_set<cb::tradable_pair> allPairs;

		for (auto& sequence : spec.sequences())
		{
			allPairs.insert(sequence.pairs().begin(), sequence.pairs().end());
		}

		spec.exchange().get_or_connect_websocket().subscribe_order_book(std::vector<cb::tradable_pair>{ allPairs.begin(), allPairs.end() });
	}

	std::this_thread::sleep_for(std::chrono::seconds(5));
}

void tri_arb_strategy::run_iteration()
{
	for (auto& spec : _specs)
	{
		cb::exchange& exchange = spec.exchange();
		double tradeValue = _options.max_trade_percent() * get_balance(exchange, _options.fiat_currency());

		for (auto& sequence : spec.sequences())
		{
			const std::unordered_map<cb::tradable_pair, cb::order_book_level> prices = get_best_order_book_prices(exchange.get_or_connect_websocket(), sequence.pairs());
			const std::unordered_map<cb::tradable_pair, double> fees = exchange.get_fees(sequence.pairs());

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
				//log_sequence_checked(sequence, tradeValue, gains.g3);
			}
		}
	}
}