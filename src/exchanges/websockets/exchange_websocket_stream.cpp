#include "exchange_websocket_stream.h"
#include "logging/logger.h"
#include "common/utils/containerutils.h"

#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	exchange_websocket_stream::exchange_websocket_stream(
		std::string_view id, 
		std::string url,
		std::unique_ptr<websocket_connection_factory> connectionFactory)
		: 
		_id{ id },
		_url{ std::move(url) },
		_connectionFactory{ std::move(connectionFactory) }
	{
		initialise_connection_factory();
	}

	void exchange_websocket_stream::initialise_connection_factory()
	{
		_connectionFactory->set_on_open([this]() { on_open(); });
		_connectionFactory->set_on_close([this]() { on_close(); });
		_connectionFactory->set_on_message([this](std::string_view message) { on_message(message); });
	}

	void exchange_websocket_stream::clear_subscriptions()
	{
		auto lockedSubscriptions = _subscriptions.unique_lock();
		auto lockedTrades = _trades.unique_lock();
		auto lockedOhlcv = _ohlcv.unique_lock();
		auto lockedOrderBooks = _orderBooks.unique_lock();

		lockedSubscriptions->clear();
		lockedTrades->clear();
		lockedOhlcv->clear();
		lockedOrderBooks->clear();
	}

	void exchange_websocket_stream::set_subscribed(std::string subscriptionId)
	{
		auto lockedSubscriptions = _subscriptions.unique_lock();

		if (!lockedSubscriptions->contains(subscriptionId))
		{
			lockedSubscriptions->emplace(std::move(subscriptionId));
		}
	}

	void exchange_websocket_stream::on_open()
	{
		logger::instance().info("Websocket stream opened for exchange '{}'", _id);
	}

	void exchange_websocket_stream::on_close()
	{
		logger::instance().info("Websocket stream closed for exchange '{}'", _id);
	}

	void exchange_websocket_stream::reset()
	{
		if (_connection)
		{
			disconnect();
		}

		_connection = _connectionFactory->create_connection(_url.data());
	}

	void exchange_websocket_stream::disconnect()
	{
		_connection->close();
		clear_subscriptions();
	}

	ws_connection_status exchange_websocket_stream::connection_status() const
	{
		if (!_connection)
		{
			return ws_connection_status::CLOSED;
		}

		return _connection->connection_status();
	}

	void exchange_websocket_stream::set_unsubscribed(std::string subscriptionId, websocket_channel channel)
	{
		auto lockedSubscriptions = _subscriptions.unique_lock();

		lockedSubscriptions->erase(subscriptionId);

		switch (channel)
		{
		case websocket_channel::TRADE:
		{
			auto lockedTrades = _trades.unique_lock();
			lockedTrades->erase(subscriptionId);
			break;
		}
		case websocket_channel::OHLCV:
		{
			auto lockedOhlcv = _ohlcv.unique_lock();
			lockedOhlcv->erase(subscriptionId);
			break;
		}
		case websocket_channel::ORDER_BOOK:
		{
			auto lockedOrderBooks = _orderBooks.unique_lock();
			lockedOrderBooks->erase(subscriptionId);
			break;
		}
		default:
			throw std::invalid_argument{ "Websocket channel not recognized" };
		}
	}

	void exchange_websocket_stream::update_trade(std::string subscriptionId, trade_update trade)
	{
		auto lockedTrades = _trades.unique_lock();
		lockedTrades->insert_or_assign(subscriptionId, std::move(trade));

		set_subscribed(std::move(subscriptionId));
	}

	void exchange_websocket_stream::update_ohlcv(std::string subscriptionId, ohlcv_data ohlcvData)
	{
		auto lockedOhlcv = _ohlcv.unique_lock();
		lockedOhlcv->insert_or_assign(subscriptionId, std::move(ohlcvData));

		set_subscribed(std::move(subscriptionId));
	}

	void exchange_websocket_stream::initialise_order_book(std::string subscriptionId, order_book_cache cache)
	{
		auto lockedOrderBooks = _orderBooks.unique_lock();
		lockedOrderBooks->insert_or_assign(subscriptionId, std::move(cache));

		set_subscribed(std::move(subscriptionId));
	}

	void exchange_websocket_stream::update_order_book(std::string subscriptionId, order_book_entry entry)
	{
		auto lockedOrderBooks = _orderBooks.unique_lock();

		if (!lockedOrderBooks->contains(subscriptionId))
		{
			lockedOrderBooks->insert_or_assign(subscriptionId, order_book_cache{ {}, {} });
			set_subscribed(subscriptionId);
		}

		auto it = lockedOrderBooks->find(subscriptionId);
		it->second.update_cache(std::move(entry));
	}

	subscription_status exchange_websocket_stream::get_subscription_status(const unique_websocket_subscription& subscription) const
	{
		std::string subId{ generate_subscription_id(subscription) };

		auto lockedSubscriptionStatus = _subscriptions.shared_lock();

		return lockedSubscriptionStatus->contains(subId)
			? subscription_status::SUBSCRIBED
			: subscription_status::UNSUBSCRIBED;
	}

	order_book_state exchange_websocket_stream::get_order_book(const tradable_pair& pair, int depth) const
	{
		std::string subId{ generate_subscription_id(unique_websocket_subscription::create_order_book_sub(pair)) };

		auto lockedOrderBooks = _orderBooks.shared_lock();
		auto it = lockedOrderBooks->find(subId);

		if (it != lockedOrderBooks->end())
		{
			return it->second.snapshot(depth);
		}

		return order_book_state{ {}, {} };
	}

	trade_update exchange_websocket_stream::get_last_trade(const tradable_pair& pair) const
	{
		std::string subId{ generate_subscription_id(unique_websocket_subscription::create_trade_sub(pair)) };
		
		auto lockedTrades = _trades.shared_lock();
		return find_or_default(*lockedTrades, subId, trade_update{0, 0, 0});
	}

	ohlcv_data exchange_websocket_stream::get_last_candle(const tradable_pair& pair, ohlcv_interval interval) const
	{
		std::string subId{ generate_subscription_id(unique_websocket_subscription::create_ohlcv_sub(pair, interval)) };
		
		auto lockedOhlcv = _ohlcv.shared_lock();
		return find_or_default(*lockedOhlcv, subId, ohlcv_data{});
	}
}