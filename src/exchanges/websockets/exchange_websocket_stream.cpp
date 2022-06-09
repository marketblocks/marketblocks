#include "exchange_websocket_stream.h"
#include "logging/logger.h"
#include "common/utils/containerutils.h"

#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	exchange_websocket_stream::exchange_websocket_stream(
		std::string_view id, 
		std::string_view url,
		std::unique_ptr<websocket_connection_factory> connectionFactory)
		: 
		_id{ id },
		_url{ url },
		_connectionFactory{ std::move(connectionFactory) }
	{
		initialise_connection_factory();
		reset();
	}

	void exchange_websocket_stream::initialise_connection_factory()
	{
		_connectionFactory->set_on_open([this]() { on_open(); });
		_connectionFactory->set_on_close([this](std::error_code error) { on_close(error); });
		_connectionFactory->set_on_fail([this](std::error_code error) { on_fail(error); });
		_connectionFactory->set_on_message([this](std::string_view message) { on_message(message); });
	}

	void exchange_websocket_stream::on_open() const
	{

	}

	void exchange_websocket_stream::on_close(std::error_code error) const
	{

	}

	void exchange_websocket_stream::on_fail(std::error_code error) const
	{

	}

	void exchange_websocket_stream::reset()
	{
		if (_connection->connection_status() != ws_connection_status::CLOSED)
		{
			disconnect();
		}

		_connection = _connectionFactory->create_connection(_url.data());
	}

	void exchange_websocket_stream::disconnect()
	{
		_connection->close();
	}

	ws_connection_status exchange_websocket_stream::connection_status() const
	{
		return _connection->connection_status();
	}

	void exchange_websocket_stream::update_subscription_status(std::string subscriptionId, websocket_channel channel, subscription_status status)
	{
		if (status == subscription_status::UNSUBSCRIBED)
		{
			_subscriptionStatus.erase(subscriptionId);

			switch (channel)
			{
			case websocket_channel::PRICE:
				_prices.erase(subscriptionId);
				break;
			case websocket_channel::OHLCV:
				_ohlcv.erase(subscriptionId);
				break;
			case websocket_channel::ORDER_BOOK:
				_orderBooks.erase(subscriptionId);
				break;
			default:
				throw std::invalid_argument{ "Websocket channel not recognized" };
			}
		}
		else
		{
			_subscriptionStatus.insert_or_assign(subscriptionId, status);
		}
	}

	void exchange_websocket_stream::update_price(std::string subscriptionId, double price)
	{
		_prices.insert_or_assign(std::move(subscriptionId), price);
	}

	void exchange_websocket_stream::update_ohlcv(std::string subscriptionId, ohlcv_data ohlcvData)
	{
		_ohlcv.insert_or_assign(std::move(subscriptionId), std::move(ohlcvData));
	}

	void exchange_websocket_stream::initialise_order_book(std::string subscriptionId, order_book_cache cache)
	{
		_orderBooks.insert_or_assign(std::move(subscriptionId), std::move(cache));
	}

	void exchange_websocket_stream::update_order_book(std::string subscriptionId, order_book_entry entry)
	{
		auto it = _orderBooks.find(subscriptionId);

		if (it != _orderBooks.end())
		{
			it->second.update_cache(std::move(entry));
		}

		throw mb_exception{ "Order book must be initialised before update" };
	}

	subscription_status exchange_websocket_stream::get_subscription_status(const unique_websocket_subscription& subscription) const
	{
		std::string subId{ generate_subscription_id(subscription) };
		return find_or_default(_subscriptionStatus, subId, subscription_status::UNSUBSCRIBED);
	}

	order_book_state exchange_websocket_stream::get_order_book(const tradable_pair& pair, int depth) const
	{
		std::string subId{ generate_subscription_id(unique_websocket_subscription::create_order_book_sub(pair)) };

		auto it = _orderBooks.find(subId);

		if (it != _orderBooks.end())
		{
			return it->second.snapshot(depth);
		}

		return order_book_state{ {}, {} };
	}

	double exchange_websocket_stream::get_price(const tradable_pair& pair) const
	{
		std::string subId{ generate_subscription_id(unique_websocket_subscription::create_price_sub(pair)) };
		return find_or_default(_prices, subId, 0.0);
	}

	ohlcv_data exchange_websocket_stream::get_last_candle(const tradable_pair& pair, ohlcv_interval interval) const
	{
		std::string subId{ generate_subscription_id(unique_websocket_subscription::create_ohlcv_sub(pair, interval)) };
		return find_or_default(_ohlcv, subId, ohlcv_data{ 0, 0, 0, 0, 0 });
	}
}