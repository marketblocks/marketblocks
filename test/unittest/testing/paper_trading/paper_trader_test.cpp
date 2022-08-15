#include <gtest/gtest.h>

#include "unittest/mocks.h"
#include "testing/paper_trading/paper_trade_api.h"
#include "common/exceptions/mb_exception.h"
#include "common/utils/containerutils.h"

namespace
{
	using namespace mb;
	using namespace mb::test;

	using ::testing::Return;

	class PaperTradeApiTest : public testing::Test
	{
	protected:
		static constexpr double InitialGbpBalance = 100.0;
		static constexpr double InitialBtcBalance = 1.5;
		static constexpr double DefaultPrice = 20.0;
		static constexpr double Fee = 0.1;
		
		tradable_pair _pair;
		mock_websocket_stream* _mockWebsocketStream;
		std::unique_ptr<paper_trade_api> _paperTradeApi;

		void set_price(double price, bool fireHandler)
		{
			trade_update update{ now_t(), price, 1.0 };

			ON_CALL(*_mockWebsocketStream, get_last_trade)
				.WillByDefault(Return(update));

			if (fireHandler)
			{
				_mockWebsocketStream->expose_fire_trade_update(trade_update_message{ _pair, std::move(update) });
			}
		};

	public:
		PaperTradeApiTest()
			: _pair{ "BTC", "GBP" }
		{}

		void SetUp() override
		{
			std::unique_ptr<mock_websocket_stream> mockWebsocketStream{ std::make_unique<mock_websocket_stream>() };
			_mockWebsocketStream = mockWebsocketStream.get();

			set_price(DefaultPrice, false);

			std::unordered_map<std::string, double> initialBalances
			{
				{ "GBP", InitialGbpBalance },
				{ "BTC", InitialBtcBalance }
			};

			_paperTradeApi = std::make_unique<paper_trade_api>(
				paper_trading_config{ Fee, std::move(initialBalances) },
				std::move(mockWebsocketStream),
				"",
				now_t);
		};
	};
}

namespace mb::test
{
	TEST_F(PaperTradeApiTest, AddBuyOrderCorrectlyAdjustsBalances)
	{
		order_request orderRequest{ create_market_order(this->_pair, trade_action::BUY, 2.0) };
		this->_paperTradeApi->add_order(orderRequest);

		std::unordered_map<std::string,double> balances = this->_paperTradeApi->get_balances();
		EXPECT_DOUBLE_EQ(balances.at(orderRequest.pair().asset()), 3.5);
		EXPECT_DOUBLE_EQ(balances.at(orderRequest.pair().price_unit()), 59.96);
	}

	TEST_F(PaperTradeApiTest, AddSellOrderCorrectlyAdjustsBalances)
	{
		order_request orderRequest{ create_market_order(this->_pair, trade_action::SELL, 1.0) };
		this->_paperTradeApi->add_order(orderRequest);

		std::unordered_map<std::string,double> balances = this->_paperTradeApi->get_balances();
		EXPECT_DOUBLE_EQ(balances.at(orderRequest.pair().asset()), 0.5);
		EXPECT_DOUBLE_EQ(balances.at(orderRequest.pair().price_unit()), 119.98);
	}

	TEST_F(PaperTradeApiTest, AddBuyOrderThrowsIfInsufficientFunds)
	{
		order_request orderRequest{ create_market_order(this->_pair, trade_action::BUY, 10) };
		EXPECT_THROW(this->_paperTradeApi->add_order(orderRequest), mb_exception);
	}

	TEST_F(PaperTradeApiTest, AddSellOrderThrowsIfInsufficientFunds)
	{
		order_request orderRequest{ create_market_order(this->_pair, trade_action::SELL, 10) };
		EXPECT_THROW(this->_paperTradeApi->add_order(orderRequest), mb_exception);
	}

	TEST_F(PaperTradeApiTest, BuyLimitOrderExecutesWhenPriceLessThanLimitPrice)
	{
		order_request orderRequest{ create_limit_order(this->_pair, trade_action::BUY, 10.0, 1.0) };
		std::string orderId{ this->_paperTradeApi->add_order(orderRequest) };

		ASSERT_EQ(order_status::OPEN, this->_paperTradeApi->get_order_status(orderId));

		set_price(5.0, true);

		ASSERT_EQ(order_status::CLOSED, this->_paperTradeApi->get_order_status(orderId));
	}

	TEST_F(PaperTradeApiTest, SellLimitOrderExecutesWhenPriceGreaterThanLimitPrice)
	{
		order_request orderRequest{ create_limit_order(this->_pair, trade_action::SELL, 40.0, 1.0) };
		std::string orderId{ this->_paperTradeApi->add_order(orderRequest) };

		ASSERT_EQ(order_status::OPEN, this->_paperTradeApi->get_order_status(orderId));

		set_price(50.0, true);

		ASSERT_EQ(order_status::CLOSED, this->_paperTradeApi->get_order_status(orderId));
	}

	TEST_F(PaperTradeApiTest, StopLossSellOrderExecutesWhenPriceLessThanStopPrice)
	{
		order_request orderRequest{ create_stop_loss_order(this->_pair, trade_action::SELL, 10.0, 1.0) };
		std::string orderId{ this->_paperTradeApi->add_order(orderRequest) };

		ASSERT_EQ(order_status::OPEN, this->_paperTradeApi->get_order_status(orderId));

		set_price(9.0, true);

		ASSERT_EQ(order_status::CLOSED, this->_paperTradeApi->get_order_status(orderId));
	}

	TEST_F(PaperTradeApiTest, StopLossBuyOrderExecutesWhenPriceGreaterThanStopPrice)
	{
		order_request orderRequest{ create_stop_loss_order(this->_pair, trade_action::BUY, 40.0, 1.0) };
		std::string orderId{ this->_paperTradeApi->add_order(orderRequest) };

		ASSERT_EQ(order_status::OPEN, this->_paperTradeApi->get_order_status(orderId));

		set_price(50.0, true);

		ASSERT_EQ(order_status::CLOSED, this->_paperTradeApi->get_order_status(orderId));
	}

	TEST_F(PaperTradeApiTest, TrailingStopLossBuyOrderExecutesWhenPriceGreaterThanMinPlusDelta)
	{
		order_request orderRequest{ create_trailing_stop_loss_order(this->_pair, trade_action::BUY, 0.1, 1.0) };
		std::string orderId{ this->_paperTradeApi->add_order(orderRequest) };

		ASSERT_EQ(order_status::OPEN, this->_paperTradeApi->get_order_status(orderId));

		set_price(21.0, true);
		ASSERT_EQ(order_status::OPEN, this->_paperTradeApi->get_order_status(orderId));

		set_price(15.0, true);
		ASSERT_EQ(order_status::OPEN, this->_paperTradeApi->get_order_status(orderId));

		set_price(17.0, true);
		ASSERT_EQ(order_status::CLOSED, this->_paperTradeApi->get_order_status(orderId));
	}

	TEST_F(PaperTradeApiTest, TrailingStopLossSellOrderExecutesWhenPriceLessThanMaxMinusDelta)
	{
		order_request orderRequest{ create_trailing_stop_loss_order(this->_pair, trade_action::SELL, 0.1, 1.0) };
		std::string orderId{ this->_paperTradeApi->add_order(orderRequest) };

		ASSERT_EQ(order_status::OPEN, this->_paperTradeApi->get_order_status(orderId));

		set_price(19.0, true);
		ASSERT_EQ(order_status::OPEN, this->_paperTradeApi->get_order_status(orderId));

		set_price(25.0, true);
		ASSERT_EQ(order_status::OPEN, this->_paperTradeApi->get_order_status(orderId));

		set_price(22.0, true);
		ASSERT_EQ(order_status::CLOSED, this->_paperTradeApi->get_order_status(orderId));
	}
}