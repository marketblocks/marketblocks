#include "exchange_websocket_stream.h"
#include "logging/logger.h"
#include "common/utils/containerutils.h"

#include "common/exceptions/not_implemented_exception.h"

namespace
{
	using namespace mb;

	std::string create_ohlcv_sub_id(std::string pairName, ohlcv_interval interval)
	{
		return std::move(pairName) + to_string(interval);
	}
}

namespace mb
{
	exchange_websocket_stream::exchange_websocket_stream(
		std::string_view id, 
		std::string url,
		char pairSeparator,
		std::unique_ptr<websocket_connection_factory> connectionFactory)
		: 
		_id{ id },
		_url{ std::move(url) },
		_pairSeparator{ pairSeparator },
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
		auto lockedTrades = _trades.unique_lock();
		auto lockedOhlcv = _ohlcv.unique_lock();
		auto lockedOrderBooks = _orderBooks.unique_lock();

		lockedTrades->clear();
		lockedOhlcv->clear();
		lockedOrderBooks->clear();
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

	void exchange_websocket_stream::subscribe(const websocket_subscription& subscription)
	{
		{
			auto lockedPairs = _pairs.unique_lock();

			for (auto& pair : subscription.pair_item())
			{
				lockedPairs->try_emplace(pair.to_string(_pairSeparator), pair);
			}
		}

		send_subscribe(subscription);
	}

	void exchange_websocket_stream::unsubscribe(const websocket_subscription& subscription)
	{
		send_unsubscribe(subscription);
	}

	void exchange_websocket_stream::set_unsubscribed(const named_subscription& subscription)
	{
		switch (subscription.channel())
		{
		case websocket_channel::TRADE:
		{
			auto lockedTrades = _trades.unique_lock();
			lockedTrades->erase(subscription.pair_item());
			break;
		}
		case websocket_channel::OHLCV:
		{
			auto lockedOhlcv = _ohlcv.unique_lock();
			lockedOhlcv->erase(create_ohlcv_sub_id(subscription.pair_item(), subscription.get_ohlcv_interval()));
			break;
		}
		case websocket_channel::ORDER_BOOK:
		{
			auto lockedOrderBooks = _orderBooks.unique_lock();
			lockedOrderBooks->erase(subscription.pair_item());
			break;
		}
		default:
			throw std::invalid_argument{ "Websocket channel not recognized" };
		}
	}

	void exchange_websocket_stream::update_trade(std::string pairName, trade_update trade)
	{
		auto lockedTrades = _trades.unique_lock();
		lockedTrades->insert_or_assign(pairName, trade);
		
		fire_trade_update(trade_update_message{ _pairs.shared_lock()->at(pairName), std::move(trade)});
	}

	void exchange_websocket_stream::update_ohlcv(std::string pairName, ohlcv_interval interval, ohlcv_data ohlcvData)
	{
		auto lockedOhlcv = _ohlcv.unique_lock();
		lockedOhlcv->insert_or_assign(create_ohlcv_sub_id(pairName, interval), ohlcvData);

		fire_ohlcv_update(ohlcv_update_message{ _pairs.shared_lock()->at(pairName), interval, std::move(ohlcvData) });
	}

	void exchange_websocket_stream::initialise_order_book(std::string pairName, order_book_cache cache)
	{
		auto lockedOrderBooks = _orderBooks.unique_lock();
		lockedOrderBooks->insert_or_assign(std::move(pairName), std::move(cache));

		fire_order_book_update(order_book_update_message{ _pairs.shared_lock()->at(pairName), cache.snapshot() });
	}

	void exchange_websocket_stream::update_order_book(std::string pairName, std::time_t timeStamp, order_book_entry entry)
	{
		auto lockedOrderBooks = _orderBooks.unique_lock();

		if (!contains(*lockedOrderBooks, pairName))
		{
			lockedOrderBooks->insert_or_assign(pairName, order_book_cache{ 0, {}, {} });
		}

		auto cacheIt = lockedOrderBooks->find(pairName);
		cacheIt->second.update_cache(timeStamp, std::move(entry));

		fire_order_book_update(order_book_update_message{ _pairs.shared_lock()->at(pairName), cacheIt->second.snapshot() });
	}

	subscription_status exchange_websocket_stream::get_subscription_status(const unique_websocket_subscription& subscription) const
	{
		std::string pairName{ subscription.pair_item().to_string(_pairSeparator) };
		
		bool subscribed = false;
		switch (subscription.channel())
		{
		case websocket_channel::TRADE:
		{
			subscribed = contains(*_trades.shared_lock(), pairName);
			break;
		}
		case websocket_channel::OHLCV:
		{
			subscribed = contains(*_ohlcv.shared_lock(), create_ohlcv_sub_id(std::move(pairName), subscription.get_ohlcv_interval()));
			break;
		}
		case websocket_channel::ORDER_BOOK:
		{
			subscribed = contains(*_orderBooks.shared_lock(), pairName);
			break;
		}
		default:
			subscribed = false;
		}

		return subscribed
			? subscription_status::SUBSCRIBED
			: subscription_status::UNSUBSCRIBED;
	}

	order_book_state exchange_websocket_stream::get_order_book(const tradable_pair& pair, int depth) const
	{
		auto lockedOrderBooks = _orderBooks.shared_lock();
		auto it = lockedOrderBooks->find(pair.to_string(_pairSeparator));

		if (it != lockedOrderBooks->end())
		{
			return it->second.snapshot(depth);
		}

		return order_book_state{ 0, {}, {} };
	}

	trade_update exchange_websocket_stream::get_last_trade(const tradable_pair& pair) const
	{
		auto lockedTrades = _trades.shared_lock();
		return find_or_default(*lockedTrades, pair.to_string(_pairSeparator), trade_update{0, 0, 0});
	}

	ohlcv_data exchange_websocket_stream::get_last_candle(const tradable_pair& pair, ohlcv_interval interval) const
	{
		std::string subId{ create_ohlcv_sub_id(pair.to_string(_pairSeparator), interval) };
		
		auto lockedOhlcv = _ohlcv.shared_lock();
		return find_or_default(*lockedOhlcv, subId, ohlcv_data{});
	}
}