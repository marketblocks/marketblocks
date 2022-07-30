#include "coinbase_websocket.h"

#include "common/utils/containerutils.h"
#include "common/json/json.h"
#include "common/utils/timeutils.h"
#include "logging/logger.h"
#include "exchanges/exchange_ids.h"

namespace
{
	using namespace mb;

	constexpr const char PAIR_SEPARATOR = '-';

	std::string create_message(std::string_view type, std::string channel, const std::vector<tradable_pair>& pairs)
	{
		std::vector<std::string> tradablePairList{ to_vector<std::string>(pairs, [](const tradable_pair& pair)
		{
			return pair.to_string(PAIR_SEPARATOR);
		}) };

		return json_writer{}
			.add("type", type)
			.add("product_ids", std::move(tradablePairList))
			.add("channels", std::vector<std::string>{ channel })
			.to_string();
	}

	std::string get_channel(websocket_channel channel)
	{
		switch (channel)
		{
		case websocket_channel::TRADE:
			return "ticker";
		case websocket_channel::ORDER_BOOK:
			return "level2";
		default:
			throw mb_exception{ fmt::format("Websocket channel not supported on Coinbase") };
		}
	}

	order_book_entry create_order_book_entry(order_book_side side, const json_element& json, int offset = 0)
	{
		return order_book_entry
		{
			std::stod(json.get<std::string>(0 + offset)),
			std::stod(json.get<std::string>(1 + offset)),
			side
		};
	}

	std::time_t parse_time_t(const std::string& source)
	{
		return to_time_t(source, "%Y-%m-%dT%T");
	}
}

namespace mb::internal
{
	coinbase_websocket_stream::coinbase_websocket_stream(
		std::unique_ptr<websocket_connection_factory> connectionFactory,
		std::unique_ptr<market_api> marketApi)
		: 
		exchange_websocket_stream
		{ 
			exchange_ids::COINBASE, 
			"wss://ws-feed.exchange.coinbase.com", 
			PAIR_SEPARATOR,
			std::move(connectionFactory) 
		},
		_ohlcvSubscriptionService{ ohlcv_subscription_service
			{
				std::move(marketApi),
				[this](std::string pairName, ohlcv_interval interval, ohlcv_data ohlcv) { update_ohlcv(std::move(pairName), interval, std::move(ohlcv)); },
				PAIR_SEPARATOR
			}}
	{}

	void coinbase_websocket_stream::process_trade_message(const json_document& json)
	{
		double price{ std::stod(json.get<std::string>("price")) };
		double volume{ std::stod(json.get<std::string>("last_size")) };
		std::time_t time{ parse_time_t(json.get<std::string>("time")) };

		std::string pairName{ json.get<std::string>("product_id") };
		update_trade(pairName, trade_update{time, price, volume});

		auto lockedOhlcvSubscriptions = _ohlcvSubscriptionService.unique_lock();

		if (lockedOhlcvSubscriptions->is_subscribed(pairName))
		{
			lockedOhlcvSubscriptions->update_ohlcv(std::move(pairName), time, price, volume);
		}
	}

	void coinbase_websocket_stream::process_order_book_initialisation(const json_document& json)
	{
		json_element asks{ json.element("asks") };
		json_element bids{ json.element("bids") };

		int maxDepth = std::max(asks.size(), bids.size());

		ask_cache askCache;
		bid_cache bidCache;
		std::time_t timeStamp{ now_t() };

		for (int i = 0; i < maxDepth; ++i)
		{
			if (i < asks.size())
			{
				askCache.emplace(create_order_book_entry(order_book_side::ASK, asks.element(i)));
			}

			if (i < bids.size())
			{
				bidCache.emplace(create_order_book_entry(order_book_side::BID, bids.element(i)));
			}
		}

		std::string pairName{ json.get<std::string>("product_id") };
		initialise_order_book(std::move(pairName), order_book_cache{ timeStamp, std::move(askCache), std::move(bidCache) });
	}

	void coinbase_websocket_stream::process_order_book_update(const json_document& json)
	{
		std::time_t timeStamp{ parse_time_t(json.get<std::string>("time")) };
		json_element changesElement{ json.element("changes") };
		std::string pairName{ json.get<std::string>("product_id") };

		for (auto it = changesElement.begin(); it != changesElement.end(); ++it)
		{
			json_element entryElement{ it.value() };

			order_book_side side = entryElement.get<std::string>(0) == "buy"
				? order_book_side::BID
				: order_book_side::ASK;

			update_order_book(pairName, timeStamp, create_order_book_entry(side, entryElement, 1));
		}
	}

	void coinbase_websocket_stream::on_message(std::string_view message)
	{
		json_document json{ parse_json(message) };

		std::string messageType{ json.get<std::string>("type") };
		
		if (messageType == "ticker")
		{
			process_trade_message(json);
		}
		else if (messageType == "snapshot")
		{
			process_order_book_initialisation(json);
		}
		else if (messageType == "l2update")
		{
			process_order_book_update(json);
		}
	}

	void coinbase_websocket_stream::send_subscribe(const websocket_subscription& subscription)
	{
		if (subscription.channel() == websocket_channel::OHLCV)
		{
			send_subscribe(websocket_subscription::create_trade_sub(subscription.pair_item()));
			_ohlcvSubscriptionService.unique_lock()->add_subscription(subscription);
			return;
		}

		std::string channelName{ get_channel(subscription.channel()) };
		std::string message{ create_message("subscribe", channelName, subscription.pair_item()) };

		_connection->send_message(message);
	}

	void coinbase_websocket_stream::send_unsubscribe(const websocket_subscription& subscription)
	{
		if (subscription.channel() == websocket_channel::OHLCV)
		{
			_ohlcvSubscriptionService.unique_lock()->remove_subscription(subscription);
			return;
		}

		std::string channelName{ get_channel(subscription.channel()) };
		std::string message{ create_message("unsubscribe", std::move(channelName), subscription.pair_item()) };

		_connection->send_message(std::move(message));
	}
}