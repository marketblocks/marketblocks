#include <gtest/gtest.h>

#include "../mocks.h"
#include "strategies/tri_arb.h"
#include "utils/containerutils.h"

using testing::Return;
using testing::Matcher;
using testing::AllOf;
using testing::Property;
using testing::Contains;
using testing::DoubleEq;

namespace
{
	auto IsTradeDescription(const TradeDescription& description)
	{
		return AllOf(
			Property(&TradeDescription::pair, description.pair()),
			Property(&TradeDescription::action, description.action()),
			Property(&TradeDescription::asset_price, DoubleEq(description.asset_price())),
			Property(&TradeDescription::volume, DoubleEq(description.volume())));
	}

	auto IsTriArbSequence(const TriArbSequence& sequence)
	{
		return AllOf(
			Property(&TriArbSequence::first, sequence.first()),
			Property(&TriArbSequence::middle, sequence.middle()),
			Property(&TriArbSequence::last, sequence.last()));
	}

	std::string to_string(const TradeAction& action)
	{
		return action == TradeAction::BUY ? "BUY" : "SELL";
	}

	std::vector<TriArbExchangeSpec> execute_create_exchange_specs(std::vector<TradablePair> tradablePairs)
	{
		auto mockExchange = std::make_shared<testing::NiceMock<MockExchange>>();

		EXPECT_CALL(*mockExchange, get_tradable_pairs).WillOnce(Return(tradablePairs));

		std::vector<std::shared_ptr<Exchange>> exchanges
		{
			mockExchange
		};

		std::vector<TriArbExchangeSpec> specs = create_exchange_specs(exchanges, AssetSymbol{ "GBP" });

		return specs;
	}

	void setup_exchange_mock(
		const MockExchange& mockExchange,
		const std::vector<TradablePair>& tradablePairs,
		const std::vector<OrderBookLevel>& prices,
		double fee)
	{
		EXPECT_CALL(mockExchange, get_order_book)
			.WillRepeatedly(Return(
				std::unordered_map<TradablePair, OrderBookState>
		{
			{ tradablePairs[0], OrderBookState{{ prices[0] }} },
			{ tradablePairs[1], OrderBookState{{ prices[1] }} },
			{ tradablePairs[2], OrderBookState{{ prices[2] }} },
		}));

		EXPECT_CALL(mockExchange, get_balances)
			.WillRepeatedly(Return(
				std::unordered_map<AssetSymbol, double>
		{
			{ AssetSymbol{ "GBP" }, 5.0 }
		}));

		EXPECT_CALL(mockExchange, get_fees)
			.WillRepeatedly(Return(to_unordered_map<TradablePair, double>(
				tradablePairs,
				[](const TradablePair& pair) { return pair; },
				[fee](const TradablePair&) {return fee; })));
	}

	void execute_create_specs_test(
		std::vector<TradablePair> tradablePairs,
		std::vector<TradeAction> expectedActions)
	{
		std::vector<TriArbExchangeSpec> specs = execute_create_exchange_specs(tradablePairs);

		TriArbExchangeSpec& spec = specs[0];

		TriArbSequence expectedSequence
		{
			SequenceStep{ tradablePairs[0], expectedActions[0] },
			SequenceStep{ tradablePairs[1], expectedActions[1] },
			SequenceStep{ tradablePairs[2], expectedActions[2] },
			tradablePairs
		};

		EXPECT_THAT(spec.sequences(), Contains(IsTriArbSequence(expectedSequence)));
	}

	void execute_trade_sequence_test(
		const std::vector<TradablePair>& tradablePairs,
		const std::vector<TradeAction>& actions,
		const std::vector<OrderBookLevel>& prices,
		const std::vector<double>& expectedAssetPrices,
		const std::vector<double>& expectedVolumes,
		double fee = 0.0)
	{
		std::vector<TriArbSequence> sequences
		{
			TriArbSequence
			{
				SequenceStep{tradablePairs[0], actions[0]},
				SequenceStep{tradablePairs[1], actions[1]},
				SequenceStep{tradablePairs[2], actions[2]},
				tradablePairs
			}
		};

		auto mockExchange = std::make_shared<testing::NiceMock<MockExchange>>();

		// Setup Mocks
		setup_exchange_mock(*mockExchange, tradablePairs, prices, fee);

		EXPECT_CALL(*mockExchange, trade(IsTradeDescription(TradeDescription{ tradablePairs[1], actions[1], expectedAssetPrices[1], expectedVolumes[1] }))).Times(1);
		EXPECT_CALL(*mockExchange, trade(IsTradeDescription(TradeDescription{ tradablePairs[2], actions[2], expectedAssetPrices[2], expectedVolumes[2] }))).Times(1);
		EXPECT_CALL(*mockExchange, trade(IsTradeDescription(TradeDescription{ tradablePairs[0], actions[0], expectedAssetPrices[0], expectedVolumes[0] }))).Times(1);

		TriArbExchangeSpec spec{ mockExchange, sequences };
		TriArbStrategy strategy{ std::vector<TriArbExchangeSpec>{spec}, TradingOptions{ 1.0, AssetSymbol{"GBP"} } };

		strategy.run_iteration();
	}
}

void PrintTo(const TradeDescription& description, std::ostream* os)
{
	*os << "Tradable Pair: " << description.pair().asset().get() << "/" << description.pair().price_unit().get() << std::endl;
	*os << "Action: " << to_string(description.action()) << std::endl;
	*os << "Asset Price: " << description.asset_price() << std::endl;
	*os << "Volume: " << description.volume() << std::endl;
}

void PrintTo(const SequenceStep& step, std::ostream* os)
{
	*os << "Tradable Pair: " << step.pair().asset().get() << "/" << step.pair().price_unit().get() << std::endl;
	*os << "Action: " << to_string(step.action()) << std::endl;
}
TEST(TriArb, CreateSpecFindsBothSequences)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"GBP"} },
		TradablePair{ AssetSymbol{"ETH"}, AssetSymbol{"BTC"} },
		TradablePair{ AssetSymbol{"ETH"}, AssetSymbol{"GBP"} },
	};

	std::vector<TriArbExchangeSpec> specs = execute_create_exchange_specs(tradablePairs);

	ASSERT_EQ(specs.size(), 1);
	ASSERT_EQ(specs[0].sequences().size(), 2);
}

TEST(TriArb, CreateSpecDoesNotTakeNonGbpPairs)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"USD"} },
		TradablePair{ AssetSymbol{"ETH"}, AssetSymbol{"BTC"} },
		TradablePair{ AssetSymbol{"ETH"}, AssetSymbol{"USD"} },
	};

	std::vector<TriArbExchangeSpec> specs = execute_create_exchange_specs(tradablePairs);

	ASSERT_EQ(specs.size(), 1);
	ASSERT_EQ(specs[0].sequences().size(), 0);
}

TEST(TriArb, CreateSpecsFindsBBB)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"EUR"}, AssetSymbol{"GBP"} },
		TradablePair{ AssetSymbol{"USD"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"USD"} },
	};

	std::vector<TradeAction> expectedActions
	{
		TradeAction::BUY,
		TradeAction::BUY,
		TradeAction::BUY
	};

	execute_create_specs_test(tradablePairs, expectedActions);
}

TEST(TriArb, CreateSpecsFindsBBS)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"GBP"} },
		TradablePair{ AssetSymbol{"ETH"}, AssetSymbol{"BTC"} },
		TradablePair{ AssetSymbol{"ETH"}, AssetSymbol{"GBP"} },
	};

	std::vector<TradeAction> expectedActions
	{
		TradeAction::BUY,
		TradeAction::BUY,
		TradeAction::SELL
	};

	execute_create_specs_test(tradablePairs, expectedActions);
}

TEST(TriArb, CreateSpecsFindsBSB)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"GBP"} },
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"USD"} },
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"USD"} },
	};

	std::vector<TradeAction> expectedActions
	{
		TradeAction::BUY,
		TradeAction::SELL,
		TradeAction::BUY
	};

	execute_create_specs_test(tradablePairs, expectedActions);
}

TEST(TriArb, CreateSpecsFindsBSS)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"GBP"} },
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"EUR"}, AssetSymbol{"GBP"} },
	};

	std::vector<TradeAction> expectedActions
	{
		TradeAction::BUY,
		TradeAction::SELL,
		TradeAction::SELL
	};

	execute_create_specs_test(tradablePairs, expectedActions);
}

TEST(TriArb, CreateSpecsFindsSBB)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"USD"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"USD"} },
	};

	std::vector<TradeAction> expectedActions
	{
		TradeAction::SELL,
		TradeAction::BUY,
		TradeAction::BUY
	};

	execute_create_specs_test(tradablePairs, expectedActions);
}

TEST(TriArb, CreateSpecsFindsSBS)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"USD"} },
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"USD"} },
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"GBP"} },
	};

	std::vector<TradeAction> expectedActions
	{
		TradeAction::SELL,
		TradeAction::BUY,
		TradeAction::SELL
	};

	execute_create_specs_test(tradablePairs, expectedActions);
}

TEST(TriArb, CreateSpecsFindsSSB)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"EUR"}, AssetSymbol{"USD"} },
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"USD"} },
	};

	std::vector<TradeAction> expectedActions
	{
		TradeAction::SELL,
		TradeAction::SELL,
		TradeAction::BUY
	};

	execute_create_specs_test(tradablePairs, expectedActions);
}

TEST(TriArb, CreateSpecsFindsSSS)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"EUR"}, AssetSymbol{"USD"} },
		TradablePair{ AssetSymbol{"USD"}, AssetSymbol{"GBP"} },
	};

	std::vector<TradeAction> expectedActions
	{
		TradeAction::SELL,
		TradeAction::SELL,
		TradeAction::SELL
	};

	execute_create_specs_test(tradablePairs, expectedActions);
}

TEST(TriArb, IterationExecutesTradesBBB)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"EUR"}, AssetSymbol{"GBP"} },
		TradablePair{ AssetSymbol{"USD"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"USD"} },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::BUY,
		TradeAction::BUY,
		TradeAction::BUY
	};

	std::vector<OrderBookLevel> prices
	{
		OrderBookLevel{ OrderBookEntry{0.8, 6.25}, OrderBookEntry{0.0, 0.0} },
		OrderBookLevel{ OrderBookEntry{0.5, 12.5}, OrderBookEntry{0.0, 0.0} },
		OrderBookLevel{ OrderBookEntry{1.25, 10.0}, OrderBookEntry{0.0, 0.0} }
	};

	std::vector<double> expectedAssetPrices{ 0.8, 0.5, 1.25 };

	std::vector<double> expectedVolumes{ 6.25, 12.5, 10.0 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		prices,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesBBS)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"GBP"} },
		TradablePair{ AssetSymbol{"ETH"}, AssetSymbol{"BTC"} },
		TradablePair{ AssetSymbol{"ETH"}, AssetSymbol{"GBP"} },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::BUY,
		TradeAction::BUY,
		TradeAction::SELL
	};

	std::vector<OrderBookLevel> prices
	{
		OrderBookLevel{ OrderBookEntry{5.0, 1.0}, OrderBookEntry{0.0, 0.0} },
		OrderBookLevel{ OrderBookEntry{0.5, 2.0}, OrderBookEntry{0.0, 0.0} },
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{3.0, 2.0} }
	};
	
	std::vector<double> expectedAssetPrices{ 5.0, 0.5, 3.0 };

	std::vector<double> expectedVolumes{ 1.0, 2.0, 2.0 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		prices,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesBSB)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"GBP"} },
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"USD"} },
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"USD"} },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::BUY,
		TradeAction::SELL,
		TradeAction::BUY
	};

	std::vector<OrderBookLevel> prices
	{
		OrderBookLevel{ OrderBookEntry{10.0, 0.5}, OrderBookEntry{0.0, 0.0} },
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{12.0, 0.5} },
		OrderBookLevel{ OrderBookEntry{0.8, 7.5}, OrderBookEntry{0.0, 0.0} }
	};

	std::vector<double> expectedAssetPrices{ 10.0, 12.0, 0.8 };

	std::vector<double> expectedVolumes{ 0.5, 0.5, 7.5 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		prices,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesBSS)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"GBP"} },
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"EUR"}, AssetSymbol{"GBP"} },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::BUY,
		TradeAction::SELL,
		TradeAction::SELL
	};

	std::vector<OrderBookLevel> prices
	{
		OrderBookLevel{ OrderBookEntry{10.0, 0.5}, OrderBookEntry{0.0, 0.0} },
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{12.0, 0.5} },
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{1.5, 6} }
	};

	std::vector<double> expectedAssetPrices{ 10.0, 12.0, 1.5 };

	std::vector<double> expectedVolumes{ 0.5, 0.5, 6 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		prices,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesSBB)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"USD"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"USD"} },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::SELL,
		TradeAction::BUY,
		TradeAction::BUY
	};

	std::vector<OrderBookLevel> prices
	{
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{1.5, 5.0} },
		OrderBookLevel{ OrderBookEntry{1.25, 6.0}, OrderBookEntry{0.0, 0.0} },
		OrderBookLevel{ OrderBookEntry{0.8, 7.5}, OrderBookEntry{0.0, 0.0} }
	};

	std::vector<double> expectedAssetPrices{ 1.5, 1.25, 0.8 };

	std::vector<double> expectedVolumes{ 5.0, 6.0, 7.5 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		prices,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesSBS)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"USD"} },
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"USD"} },
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"GBP"} },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::SELL,
		TradeAction::BUY,
		TradeAction::SELL
	};

	std::vector<OrderBookLevel> prices
	{
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{1.25, 5.0} },
		OrderBookLevel{ OrderBookEntry{10.0, 0.625}, OrderBookEntry{0.0, 0.0} },
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{10.0, 0.625} }
	};

	std::vector<double> expectedAssetPrices{ 1.25, 10.0, 10 };

	std::vector<double> expectedVolumes{ 5.0, 0.625, 0.625 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		prices,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesSSB)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"EUR"}, AssetSymbol{"USD"} },
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"USD"} },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::SELL,
		TradeAction::SELL,
		TradeAction::BUY
	};

	std::vector<OrderBookLevel> prices
	{
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{1.5, 5.0} },
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{1.25, 7.5} },
		OrderBookLevel{ OrderBookEntry{1.0, 9.375}, OrderBookEntry{0.0, 0.0} }
	};

	std::vector<double> expectedAssetPrices{ 1.5, 1.25, 1.0 };

	std::vector<double> expectedVolumes{ 5.0, 7.5, 9.375 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		prices,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesSSS)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"EUR"}, AssetSymbol{"USD"} },
		TradablePair{ AssetSymbol{"USD"}, AssetSymbol{"GBP"} },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::SELL,
		TradeAction::SELL,
		TradeAction::SELL
	};

	std::vector<OrderBookLevel> prices
	{
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{1.5, 5.0} },
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{1.5, 7.5} },
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{0.8, 11.25} }
	};

	std::vector<double> expectedAssetPrices{ 1.5, 1.5, 0.8 };

	std::vector<double> expectedVolumes{ 5.0, 7.5, 11.25 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		prices,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, CalculateCorrectTradesWithFeesBSS)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"GBP"} },
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"EUR"}, AssetSymbol{"GBP"} },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::BUY,
		TradeAction::SELL,
		TradeAction::SELL
	};

	std::vector<OrderBookLevel> prices
	{
		OrderBookLevel{ OrderBookEntry{10.0, 0.495}, OrderBookEntry{0.0, 0.0} },
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{12.0, 0.495} },
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{1.5, 5.8806} }
	};

	std::vector<double> expectedAssetPrices{ 10.0, 12.0, 1.5 };

	std::vector<double> expectedVolumes{ 0.495, 0.495, 5.8806 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		prices,
		expectedAssetPrices,
		expectedVolumes,
		1.0);
}

TEST(TriArb, CalculateCorrectTradesWithFeesSBB)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"USD"}, AssetSymbol{"EUR"} },
		TradablePair{ AssetSymbol{"GBP"}, AssetSymbol{"USD"} },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::SELL,
		TradeAction::BUY,
		TradeAction::BUY
	};

	std::vector<OrderBookLevel> prices
	{
		OrderBookLevel{ OrderBookEntry{0.0, 0.0}, OrderBookEntry{1.5, 5.0} },
		OrderBookLevel{ OrderBookEntry{1.25, 5.8806}, OrderBookEntry{0.0, 0.0} },
		OrderBookLevel{ OrderBookEntry{0.8, 7.2772425}, OrderBookEntry{0.0, 0.0} }
	};

	std::vector<double> expectedAssetPrices{ 1.5, 1.25, 0.8 };

	std::vector<double> expectedVolumes{ 5.0, 5.8806, 7.2772425 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		prices,
		expectedAssetPrices,
		expectedVolumes,
		1.0);
}

TEST(TriArb, DoesNotTradeIfProfitLessThan0)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"GBP"} },
		TradablePair{ AssetSymbol{"ETH"}, AssetSymbol{"BTC"} },
		TradablePair{ AssetSymbol{"ETH"}, AssetSymbol{"GBP"} },
	};

	std::vector<OrderBookLevel> prices
	{
		OrderBookLevel{ OrderBookEntry{5.0, 1.0}, OrderBookEntry{0.0, 0.0} },
		OrderBookLevel{ OrderBookEntry{0.5, 2.0}, OrderBookEntry{0.0, 0.0} },
		OrderBookLevel{ OrderBookEntry{2.0, 2.0}, OrderBookEntry{0.0, 0.0} }
	};

	std::vector<TriArbSequence> sequences
	{
		TriArbSequence
		{
			SequenceStep{tradablePairs[0], TradeAction::BUY},
			SequenceStep{tradablePairs[1], TradeAction::BUY},
			SequenceStep{tradablePairs[2], TradeAction::SELL},
			tradablePairs
		}
	};

	auto mockExchange = std::make_shared<testing::NiceMock<MockExchange>>();

	// Setup Mocks
	setup_exchange_mock(*mockExchange, tradablePairs, prices, 0.0);

	EXPECT_CALL(*mockExchange, trade).Times(0);

	TriArbExchangeSpec spec{ mockExchange, sequences };
	TriArbStrategy strategy{ std::vector<TriArbExchangeSpec>{spec}, TradingOptions{ 1.0, AssetSymbol{"GBP"} } };

	strategy.run_iteration();
}

TEST(TriArb, DoesNotTradeIfProfitLessThanFees)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ AssetSymbol{"BTC"}, AssetSymbol{"GBP"} },
		TradablePair{ AssetSymbol{"ETH"}, AssetSymbol{"BTC"} },
		TradablePair{ AssetSymbol{"ETH"}, AssetSymbol{"GBP"} },
	};

	std::vector<OrderBookLevel> prices
	{
		OrderBookLevel{ OrderBookEntry{5.0, 0.5}, OrderBookEntry{0.0, 0.0} },
		OrderBookLevel{ OrderBookEntry{0.5, 2.0}, OrderBookEntry{12.0, 0.5} },
		OrderBookLevel{ OrderBookEntry{2.55, 2.0}, OrderBookEntry{0.0, 0.0} }
	};

	std::vector<TriArbSequence> sequences
	{
		TriArbSequence
		{
			SequenceStep{tradablePairs[0], TradeAction::BUY},
			SequenceStep{tradablePairs[1], TradeAction::BUY},
			SequenceStep{tradablePairs[2], TradeAction::SELL},
			tradablePairs
		}
	};

	auto mockExchange = std::make_shared<testing::NiceMock<MockExchange>>();

	// Setup Mocks
	constexpr double fee = 1.0;

	setup_exchange_mock(*mockExchange, tradablePairs, prices, fee);

	EXPECT_CALL(*mockExchange, trade).Times(0);

	TriArbExchangeSpec spec{ mockExchange, sequences };
	TriArbStrategy strategy{ std::vector<TriArbExchangeSpec>{spec}, TradingOptions{ 1.0, AssetSymbol{"GBP"} } };

	strategy.run_iteration();
}