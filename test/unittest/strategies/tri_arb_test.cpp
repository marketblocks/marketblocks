#include <gtest/gtest.h>

#include "../mocks.h"
#include "strategies/tri_arb.h"

using testing::Return;
using testing::Matcher;
using testing::AllOf;
using testing::Property;
using testing::Contains;

auto IsTradeDescription(const TradeDescription& description)
{
	return AllOf(
		Property(&TradeDescription::pair, description.pair()),
		Property(&TradeDescription::action, description.action()),
		Property(&TradeDescription::asset_price, description.asset_price()),
		Property(&TradeDescription::volume, description.volume()));
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

void PrintTo(const TradeDescription& description, std::ostream* os)
{
	*os << "Tradable Pair: " << description.pair().asset() << "/" << description.pair().price_unit() << std::endl;
	*os << "Action: " << to_string(description.action()) << std::endl;
	*os << "Asset Price: " << description.asset_price() << std::endl;
	*os << "Volume: " << description.volume() << std::endl;
}

void PrintTo(const SequenceStep& step, std::ostream* os)
{
	*os << "Tradable Pair: " << step.pair().asset() << "/" << step.pair().price_unit() << std::endl;
	*os << "Action: " << to_string(step.action()) << std::endl;
}

std::vector<TriArbExchangeSpec> execute_create_exchange_specs(std::vector<TradablePair> tradablePairs)
{
	auto mockMarketData = std::make_unique<testing::NiceMock<MockMarketData>>();

	EXPECT_CALL(*mockMarketData, get_tradable_pairs).WillOnce(Return(tradablePairs));

	std::vector<std::shared_ptr<Exchange>> exchanges
	{
		std::make_shared<Exchange>(std::move(mockMarketData), std::make_unique<MockTrader>())
	};

	std::vector<TriArbExchangeSpec> specs = create_exchange_specs(exchanges, "GBP");

	return specs;
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
	std::vector<TradablePair> tradablePairs,
	std::vector<TradeAction> actions,
	std::vector<PriceData> priceData,
	std::vector<double> expectedAssetPrices,
	std::vector<double> expectedVolumes)
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

	auto mockMarketData = std::make_unique<testing::NiceMock<MockMarketData>>();
	auto mockTrader = std::make_unique<testing::NiceMock<MockTrader>>();

	// Setup Mocks
	EXPECT_CALL(*mockMarketData, get_price_data)
		.WillRepeatedly(Return(
			std::unordered_map<TradablePair, PriceData>
	{
		{ tradablePairs[0], priceData[0] },
		{ tradablePairs[1], priceData[1] },
		{ tradablePairs[2], priceData[2] },
	}));

	EXPECT_CALL(*mockTrader, get_balance("GBP"))
		.WillRepeatedly(Return(5.0));

	EXPECT_CALL(*mockTrader, trade(IsTradeDescription(TradeDescription{ tradablePairs[0], actions[0], expectedAssetPrices[0], expectedVolumes[0] }))).Times(1);
	EXPECT_CALL(*mockTrader, trade(IsTradeDescription(TradeDescription{ tradablePairs[1], actions[1], expectedAssetPrices[1], expectedVolumes[1] }))).Times(1);
	EXPECT_CALL(*mockTrader, trade(IsTradeDescription(TradeDescription{ tradablePairs[2], actions[2], expectedAssetPrices[2], expectedVolumes[2] }))).Times(1);

	std::shared_ptr<Exchange> exchange = std::make_shared<Exchange>(std::move(mockMarketData), std::move(mockTrader));
	TriArbExchangeSpec spec{ exchange, sequences };
	TriArbStrategy strategy{ std::vector<TriArbExchangeSpec>{spec}, TradingOptions{ 1.0, "GBP" }};

	strategy.run_iteration();
}

TEST(TriArb, CreateSpecFindsBothSequences)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ "BTC", "GBP" },
		TradablePair{ "ETH", "BTC" },
		TradablePair{ "ETH", "GBP" },
	};

	std::vector<TriArbExchangeSpec> specs = execute_create_exchange_specs(tradablePairs);

	ASSERT_EQ(specs.size(), 1);
	ASSERT_EQ(specs[0].sequences().size(), 2);
}

TEST(TriArb, CreateSpecDoesNotTakeNonGbpPairs)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ "BTC", "USD" },
		TradablePair{ "ETH", "BTC" },
		TradablePair{ "ETH", "USD" },
	};

	std::vector<TriArbExchangeSpec> specs = execute_create_exchange_specs(tradablePairs);

	ASSERT_EQ(specs.size(), 1);
	ASSERT_EQ(specs[0].sequences().size(), 0);
}

TEST(TriArb, CreateSpecsFindsBBB)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ "EUR", "GBP" },
		TradablePair{ "USD", "EUR" },
		TradablePair{ "GBP", "USD" },
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
		TradablePair{ "BTC", "GBP" },
		TradablePair{ "ETH", "BTC" },
		TradablePair{ "ETH", "GBP" },
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
		TradablePair{ "BTC", "GBP" },
		TradablePair{ "BTC", "USD" },
		TradablePair{ "GBP", "USD" },
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
		TradablePair{ "BTC", "GBP" },
		TradablePair{ "BTC", "EUR" },
		TradablePair{ "EUR", "GBP" },
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
		TradablePair{ "GBP", "EUR" },
		TradablePair{ "USD", "EUR" },
		TradablePair{ "GBP", "USD" },
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
		TradablePair{ "GBP", "USD" },
		TradablePair{ "BTC", "USD" },
		TradablePair{ "BTC", "GBP" },
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
		TradablePair{ "GBP", "EUR" },
		TradablePair{ "EUR", "USD" },
		TradablePair{ "GBP", "USD" },
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
		TradablePair{ "GBP", "EUR" },
		TradablePair{ "EUR", "USD" },
		TradablePair{ "USD", "GBP" },
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
		TradablePair{ "EUR", "GBP" },
		TradablePair{ "USD", "EUR" },
		TradablePair{ "GBP", "USD" },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::BUY,
		TradeAction::BUY,
		TradeAction::BUY
	};

	std::vector<PriceData> priceData
	{
		PriceData{ 0.0, 0.8 },
		PriceData{ 0.0, 0.5 },
		PriceData{ 0.0, 1.25 }
	};

	std::vector<double> expectedAssetPrices{ 0.8, 0.5, 1.25 };

	std::vector<double> expectedVolumes{ 6.25, 12.5, 10.0 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		priceData,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesBBS)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ "BTC", "GBP" },
		TradablePair{ "ETH", "BTC" },
		TradablePair{ "ETH", "GBP" },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::BUY,
		TradeAction::BUY,
		TradeAction::SELL
	};

	std::vector<PriceData> priceData
	{
		PriceData{ 0.0, 5.0 },
		PriceData{ 0.0, 0.5 },
		PriceData{ 3.0, 0.0 }
	};

	std::vector<double> expectedAssetPrices{ 5.0, 0.5, 3.0 };

	std::vector<double> expectedVolumes{ 1.0, 2.0, 2.0 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		priceData,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesBSB)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ "BTC", "GBP" },
		TradablePair{ "BTC", "USD" },
		TradablePair{ "GBP", "USD" },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::BUY,
		TradeAction::SELL,
		TradeAction::BUY
	};

	std::vector<PriceData> priceData
	{
		PriceData{ 0.0, 10.0 },
		PriceData{ 12.0, 0.0 },
		PriceData{ 0.0, 0.8 }
	};

	std::vector<double> expectedAssetPrices{ 10.0, 12.0, 0.8 };

	std::vector<double> expectedVolumes{ 0.5, 0.5, 7.5 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		priceData,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesBSS)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ "BTC", "GBP" },
		TradablePair{ "BTC", "EUR" },
		TradablePair{ "EUR", "GBP" },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::BUY,
		TradeAction::SELL,
		TradeAction::SELL
	};

	std::vector<PriceData> priceData
	{
		PriceData{ 0.0, 10.0 },
		PriceData{ 12.0, 0.0 },
		PriceData{ 1.5, 0.0 }
	};

	std::vector<double> expectedAssetPrices{ 10.0, 12.0, 1.5 };

	std::vector<double> expectedVolumes{ 0.5, 0.5, 6 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		priceData,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesSBB)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ "GBP", "EUR" },
		TradablePair{ "USD", "EUR" },
		TradablePair{ "GBP", "USD" },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::SELL,
		TradeAction::BUY,
		TradeAction::BUY
	};

	std::vector<PriceData> priceData
	{
		PriceData{ 1.5, 0.0 },
		PriceData{ 0.0, 1.25 },
		PriceData{ 0.0, 0.8 }
	};

	std::vector<double> expectedAssetPrices{ 1.5, 1.25, 0.8 };

	std::vector<double> expectedVolumes{ 5.0, 6.0, 7.5 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		priceData,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesSBS)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ "GBP", "USD" },
		TradablePair{ "BTC", "USD" },
		TradablePair{ "BTC", "GBP" },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::SELL,
		TradeAction::BUY,
		TradeAction::SELL
	};

	std::vector<PriceData> priceData
	{
		PriceData{ 1.25, 0.0 },
		PriceData{ 0.0, 10.0 },
		PriceData{ 10.0, 0.0 }
	};

	std::vector<double> expectedAssetPrices{ 1.25, 10.0, 10 };

	std::vector<double> expectedVolumes{ 5.0, 0.625, 0.625 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		priceData,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesSSB)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ "GBP", "EUR" },
		TradablePair{ "EUR", "USD" },
		TradablePair{ "GBP", "USD" },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::SELL,
		TradeAction::SELL,
		TradeAction::BUY
	};

	std::vector<PriceData> priceData
	{
		PriceData{ 1.5, 0.0 },
		PriceData{ 1.25, 0.0 },
		PriceData{ 0.0, 1.0 }
	};

	std::vector<double> expectedAssetPrices{ 1.5, 1.25, 1.0 };

	std::vector<double> expectedVolumes{ 5.0, 7.5, 9.375 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		priceData,
		expectedAssetPrices,
		expectedVolumes);
}

TEST(TriArb, IterationExecutesTradesSSS)
{
	std::vector<TradablePair> tradablePairs
	{
		TradablePair{ "GBP", "EUR" },
		TradablePair{ "EUR", "USD" },
		TradablePair{ "USD", "GBP" },
	};

	std::vector<TradeAction> actions
	{
		TradeAction::SELL,
		TradeAction::SELL,
		TradeAction::SELL
	};

	std::vector<PriceData> priceData
	{
		PriceData{ 1.5, 0.0 },
		PriceData{ 1.5, 0.0 },
		PriceData{ 0.8, 0.0 }
	};

	std::vector<double> expectedAssetPrices{ 1.5, 1.5, 0.8 };

	std::vector<double> expectedVolumes{ 5.0, 7.5, 11.25 };

	execute_trade_sequence_test(
		tradablePairs,
		actions,
		priceData,
		expectedAssetPrices,
		expectedVolumes);
}