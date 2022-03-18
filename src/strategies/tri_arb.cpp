#include <iostream>
#include <algorithm>
#include <unordered_set>

#include "tri_arb.h"
#include "trading/order_book.h"
#include "trading/trade_description.h"
#include "exchanges/exchange_helpers.h"
#include "common/utils/containerutils.h"
#include "common/utils/mathutils.h"
#include "common/utils/financeutils.h"
#include "logging/logger.h"

using namespace cb;

namespace
{
	std::string to_string(const tri_arb_sequence& sequence)
	{
		return sequence.first().pair().to_standard_string() + "->" + sequence.middle().pair().to_standard_string() + "->" + sequence.last().pair().to_standard_string();
	}
}

tri_arb_spec::tri_arb_spec(
	std::shared_ptr<cb::exchange> exchange, 
	std::unordered_map<tradable_pair, std::vector<tri_arb_sequence>> sequences)
	: 
	_exchange{ exchange }, 
	_sequences{ std::move(sequences) }, 
	_orderBookMessageQueue{}
{}

const std::vector<tri_arb_sequence>& tri_arb_spec::get_sequences(const tradable_pair& pair) const
{
	auto it = _sequences.find(pair);

	if (it == _sequences.end())
	{
		return std::vector<tri_arb_sequence>{};
	}

	return it->second;
}

std::vector<tri_arb_spec> create_exchange_specs(const std::vector<std::shared_ptr<cb::exchange>>& exchanges, std::string_view fiatCurrency)
{
	std::vector<tri_arb_spec> specs;

	for (auto exchange : exchanges)
	{
		specs.emplace_back(exchange, std::unordered_map<tradable_pair, std::vector<tri_arb_sequence>>{});
	}
	/*specs.reserve(exchanges.size());

	for (std::shared_ptr<cb::exchange> exchange : exchanges)
	{
		std::vector<tradable_pair> tradablePairs{ exchange->get_tradable_pairs() };
		std::unordered_map<tradable_pair, std::vector<tri_arb_sequence>> sequences;

		for (auto& firstPair : tradablePairs)
		{
			std::vector<tradable_pair> middlePairs = copy_where<std::vector<tradable_pair>>(tradablePairs, [&firstPair](const tradable_pair& other)
				{
					return firstPair != other && (other.contains(firstPair.asset()) || other.contains(firstPair.price_unit()));
				});

			for (auto& middlePair : middlePairs)
			{
				std::vector<tradable_pair> lastPairs = copy_where<std::vector<tradable_pair>>(tradablePairs, [&firstPair, &middlePair](const tradable_pair& other)
					{
						return firstPair != other && middlePair != other && (other.contains(middlePair.asset()) || other.contains(middlePair.price_unit()));
					});

				for (auto& lastPair : lastPairs)
				{

				}
			}
		}

		for (auto& firstPair : fiatTradables)
		{
			cb::trade_action firstAction;
			std::unique_ptr<std::string> firstGainedAsset;

			if (firstPair.price_unit() == fiatCurrency)
			{
				firstAction = cb::trade_action::BUY;
				firstGainedAsset = std::make_unique<std::string>(firstPair.asset());
			}
			else
			{
				firstAction = cb::trade_action::SELL;
				firstGainedAsset = std::make_unique<std::string>(firstPair.price_unit());
			}

			std::vector<cb::tradable_pair> possibleMiddles = copy_where<std::vector<cb::tradable_pair>>(
				nonFiatTradables,
				[&firstGainedAsset](const cb::tradable_pair& pair) { return pair.contains(*firstGainedAsset); });

			for (auto& middlePair : possibleMiddles)
			{
				cb::trade_action middleAction;
				std::unique_ptr<std::string> middleGainedAsset;

				if (middlePair.price_unit() == *firstGainedAsset)
				{
					middleAction = cb::trade_action::BUY;
					middleGainedAsset = std::make_unique<std::string>(middlePair.asset());
				}
				else
				{
					middleAction = cb::trade_action::SELL;
					middleGainedAsset = std::make_unique<std::string>(middlePair.price_unit());
				}

				std::vector<cb::tradable_pair> finals = copy_where<std::vector<cb::tradable_pair>>(
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
	}*/

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

	/*void print_trade(const cb::trade_description& tradeDescription, double gain)
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
	}*/

	/*void log_trade(const tri_arb_sequence& sequence, const SequenceTrades& trades, const SequenceGains& gains, double initialTradeValue, double newBalance)
	{
		std::cout << std::endl << "TRADE:" << std::endl;

		print_sequence(sequence);

		print_trade(trades.first, gains.g1);
		print_trade(trades.middle, gains.g2);
		print_trade(trades.last, gains.g3);

		double percentageProfit = cb::calculate_percentage_diff(initialTradeValue, gains.g3);
		std::cout << "Profit: " << percentageProfit << "%" << std::endl;

		std::cout << "New Balance: " << newBalance << std::endl << std::endl;
	}*/

	/*cb::trade_description create_trade(const sequence_step& sequenceStep, const std::unordered_map<cb::tradable_pair, cb::order_book_level>& prices, double g1, double g0)
	{
		double volume = sequenceStep.action() == cb::trade_action::BUY ? g1 : g0;
		double assetPrice = select_entry(prices.at(sequenceStep.pair()), sequenceStep.action()).price();

		return cb::trade_description(cb::order_type::LIMIT, sequenceStep.pair(), sequenceStep.action(), assetPrice, volume);
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
	}*/
}

void tri_arb_strategy::initialise(const cb::strategy_initialiser& initaliser)
{
	_options = initaliser.options();
	_specs = create_exchange_specs(initaliser.exchanges(), _options.fiat_currency());

	for (auto& spec : _specs)
	{
		auto exchange = spec.exchange();

		cb::websocket_stream& websocketStream = spec.exchange()->get_websocket_stream();
		websocketStream.connect();
		websocketStream.set_order_book_message_handler([&spec](cb::tradable_pair pair) { spec.message_queue().push(std::move(pair)); });
		websocketStream.subscribe_order_book(exchange->get_tradable_pairs());
	}
}

void tri_arb_strategy::run_iteration()
{
	for (auto& spec : _specs)
	{
		if (spec.message_queue().empty())
		{
			cb::logger::instance().info("No new updates");
			return;
		}

		while (!spec.message_queue().empty())
		{
			cb::logger::instance().info("Updated order book for pair {}", spec.message_queue().front().to_standard_string());

			spec.message_queue().pop();
		}
	}

	//for (auto& spec : _specs)
	//{
	//	cb::exchange& exchange = spec.exchange();
	//	double tradeValue = _options.max_trade_percent() * get_balance(exchange, _options.fiat_currency());

	//	for (auto& sequence : spec.sequences())
	//	{
	//		try
	//		{
	//			const std::unordered_map<cb::tradable_pair, cb::order_book_level> prices = get_best_order_book_prices(exchange.get_websocket_stream(), sequence.pairs());
	//			const std::unordered_map<cb::tradable_pair, double> fees
	//			{
	//				{ sequence.pairs()[0], 0.26 },
	//				{ sequence.pairs()[1], 0.26 },
	//				{ sequence.pairs()[2], 0.26 }
	//			};
	//			//exchange.get_fees(sequence.pairs());

	//			SequenceGains gains = calculate_sequence_gains(sequence, prices, fees, tradeValue);

	//			if (gains.g3 > tradeValue)
	//			{
	//				SequenceTrades trades = create_sequence_trades(sequence, gains, prices, tradeValue);

	//				exchange.add_order(trades.first);
	//				exchange.add_order(trades.middle);
	//				exchange.add_order(trades.last);

	//				//log_trade(sequence, trades, gains, tradeValue, get_balance(exchange, _options.fiat_currency()));
	//			}
	//			else
	//			{
	//				cb::logger::instance().info("Sequence: {0}. Percentage Diff: {1}", to_string(sequence), cb::calculate_percentage_diff(tradeValue, gains.g3));
	//			}
	//		}
	//		catch (const cb::cb_exception& e)
	//		{
	//			
	//			cb::logger::instance().error("Error occurred during sequence {0}: {1}", to_string(sequence), e.what());
	//		}
	//	}
	//}
}