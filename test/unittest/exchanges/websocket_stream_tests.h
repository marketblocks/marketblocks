#pragma once

#include <gtest/gtest.h>

#include "unittest/mocks.h"
#include "unittest/assertion_helpers.h"
#include "exchange_test_common.h"
#include "exchanges/exchange.h"
#include "test_data/test_data_constants.h"
#include "common/file/file.h"

namespace mb::test
{
	using ::testing::_;

	template<typename StreamImpl>
	class WebsocketStreamTests : public testing::Test
	{
	protected:
		mock_websocket_connection* _mockConnection;
		mock_websocket_connection_factory* _mockConnectionFactory;
		std::unique_ptr<StreamImpl> _stream;

		void SetUp() override
		{
			std::unique_ptr<mock_websocket_connection_factory> mockConnectionFactory{ std::make_unique<mock_websocket_connection_factory>() };
			_mockConnectionFactory = mockConnectionFactory.get();

			EXPECT_CALL(*mockConnectionFactory, create_connection(_))
				.WillOnce([this](std::string) 
					{
						std::unique_ptr<mock_websocket_connection> mockConnection{ std::make_unique<mock_websocket_connection>() };
						_mockConnection = mockConnection.get();

						return std::move(mockConnection);
					});

			_stream = create_websocket_stream<StreamImpl>(std::move(mockConnectionFactory));
			_stream->reset();
		}

		void set_expected_message_sent(std::string_view fileName)
		{
			std::string message{ read_websockets_file(_stream->id(), fileName)};

			EXPECT_CALL(*_mockConnection, send_message(std::move(message))).Times(1);
		}

		void receive_message(std::string_view fileName)
		{
			std::string message{ read_websockets_file(_stream->id(), fileName) };
			_mockConnectionFactory->fire_on_message(message);
		}
	};

	TYPED_TEST_SUITE_P(WebsocketStreamTests);

	TYPED_TEST_P(WebsocketStreamTests, PriceSubscriptionMessageSent)
	{
		this->set_expected_message_sent("price_subscribe");
		this->_stream->subscribe(websocket_subscription::create_price_sub(
			std::vector<tradable_pair>
			{
				tradable_pair{ "BTC", "USD" },
				tradable_pair{ "ETH", "GBP" }
			}));
	}

	TYPED_TEST_P(WebsocketStreamTests, OhlcvSubscriptionMessageSent)
	{
		this->set_expected_message_sent("ohlcv_subscribe");
		this->_stream->subscribe(websocket_subscription::create_ohlcv_sub(
			std::vector<tradable_pair>
			{
				tradable_pair{ "BTC", "USD" },
				tradable_pair{ "ETH", "GBP" }
			},
			ohlcv_interval::M5));
	}

	TYPED_TEST_P(WebsocketStreamTests, UnsubscriptionMessageSent)
	{
		this->set_expected_message_sent("unsubscription");
		this->_stream->unsubscribe(websocket_subscription::create_price_sub(
			std::vector<tradable_pair>
			{
				tradable_pair{ "BTC", "USD" },
				tradable_pair{ "ETH", "GBP" }
			}));
	}

	TYPED_TEST_P(WebsocketStreamTests, PriceUpdateOnMessage)
	{
		this->receive_message("price_update");

		EXPECT_DOUBLE_EQ(6060.4, this->_stream->get_price(tradable_pair{ "BTC","USD" }));
	}

	TYPED_TEST_P(WebsocketStreamTests, OhlcvUpdateOnMessage)
	{
		this->_stream->subscribe(websocket_subscription::create_ohlcv_sub(
			{ tradable_pair{ "BTC", "USD" }}, ohlcv_interval::M5));

		ohlcv_data expectedOhlcv{ 1657043700, 19703.50, 19720.0, 19682.1, 19683.6, 3.38715290 };

		this->receive_message("ohlcv_update");

		ohlcv_data actualOhlcv{ this->_stream->get_last_candle(tradable_pair{ "BTC","USD" }, ohlcv_interval::M5) };

		assert_ohlcv_data_eq(expectedOhlcv, actualOhlcv);
	}

	REGISTER_TYPED_TEST_SUITE_P(WebsocketStreamTests,
		PriceSubscriptionMessageSent,
		OhlcvSubscriptionMessageSent,
		UnsubscriptionMessageSent,
		PriceUpdateOnMessage,
		OhlcvUpdateOnMessage);
}