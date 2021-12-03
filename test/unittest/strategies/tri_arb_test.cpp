#include <gtest/gtest.h>

#include "../mocks.h"
#include "strategies/tri_arb.h"

using testing::Return;
using testing::Matcher;
using testing::AllOf;
using testing::Property;

auto IsTradeDescription(const TradeDescription& description)
{
	return AllOf(
		Property(&TradeDescription::pair, description.pair()),
		Property(&TradeDescription::action, description.action()),
		Property(&TradeDescription::asset_price, description.asset_price()),
		Property(&TradeDescription::volume, description.volume()));
}

void PrintTo(const TradeDescription& description, std::ostream* os)
{
	std::string action = description.action() == TradeAction::BUY ? "BUY" : "SELL";

	*os << "Tradable Pair: " << description.pair().asset() << "/" << description.pair().price_unit() << std::endl;
	*os << "Action: " << action << std::endl;
	*os << "Asset Price: " << description.asset_price() << std::endl;
	*os << "Volume: " << description.volume() << std::endl;
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
	TriArbStrategy strategy{ std::vector<TriArbExchangeSpec>{spec}, TradingOptions{ 1.0 } };

	strategy.run_iteration();
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
		PriceData{ 0.625, 0.0 }
	};

	std::vector<double> expectedAssetPrices{ 1.25, 10.0, 0.625 };

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