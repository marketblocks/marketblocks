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
		case websocket_channel::PRICE:
			return "ticker";
		case websocket_channel::ORDER_BOOK:
		default:
			throw mb_exception{ std::format("Websocket channel not supported on Coinbase") };
		}
	}

	std::string generate_subscription_id(std::string symbol, std::string channel)
	{
		return std::move(symbol) + std::move(channel);
	}

	/*tradable_pair get_pair_from_id(std::string_view productId)
	{
		std::vector<std::string> parts = split(productId, PAIR_SEPARATOR);
		return tradable_pair{ parts[0], parts[1] };
	}

	std::pair<std::string, order_book_entry> construct_order_book_map_pair(const json_element& json)
	{
		std::string price{ json.get<std::string>(0) };
		order_book_entry entry
		{
			std::stod(price),
			std::stod(json.get<std::string>(1))
		};

		return std::make_pair(std::move(price), std::move(entry));
	}

	void process_order_book_snapshot_message(const json_document& json, local_order_book& localOrderBook)
	{
		std::string productId{ json.get<std::string>("product_id") };
		tradable_pair pair{ get_pair_from_id(productId) };

		json_element asks{ json.element("asks") };
		json_element bids{ json.element("bids") };

		int maxDepth = std::max(asks.size(), bids.size());

		ask_map askMap;
		bid_map bidMap;

		for (int i = 0; i < maxDepth; ++i)
		{
			if (i < asks.size())
			{
				askMap.emplace(construct_order_book_map_pair(asks.element(i)));
			}

			if (i < bids.size())
			{
				bidMap.emplace(construct_order_book_map_pair(bids.element(i)));
			}
		}

		localOrderBook.initialise_book(std::move(pair), std::move(askMap), std::move(bidMap));
		logger::instance().info("Coinbase: initialised order book for {}", productId);
	}

	void process_order_book_update_message(const json_document& json, local_order_book& localOrderBook)
	{
		tradable_pair pair{ get_pair_from_id(json.get<std::string>("product_id")) };
		json_element changes{ json.element("changes") };

		for (auto it = changes.begin(); it != changes.end(); ++it)
		{
			json_element changeElement{ it.value() };

			order_book_side side = changeElement.get<std::string>(0) == "buy" ? order_book_side::BID : order_book_side::ASK;
			std::string price{ changeElement.get<std::string>(1) };
			double volume{ std::stod(changeElement.get<std::string>(2)) };

			if (volume == 0.0)
			{
				localOrderBook.remove_from_book(pair, price, side);
				continue;
			}

			order_book_entry entry
			{
				std::stod(price),
				volume
			};

			localOrderBook.update_book(
				pair,
				order_book_cache_entry
				{
					side,
					std::move(price),
					std::move(entry)
				});
		}
	}*/
}

namespace mb::internal
{
	coinbase_websocket_stream::coinbase_websocket_stream(
		std::unique_ptr<websocket_connection_factory> connectionFactory,
		std::unique_ptr<market_api> marketApi)
		: 
		exchange_websocket_stream{ exchange_ids::COINBASE, URL, std::move(connectionFactory) },
		_ohlcvSubscriptionService{ ohlcv_subscription_service
			{
				std::move(marketApi),
				[this](std::string id, ohlcv_data ohlcv) { update_ohlcv(std::move(id), std::move(ohlcv)); },
				PAIR_SEPARATOR
			}}
	{}

	std::string coinbase_websocket_stream::generate_subscription_id(const unique_websocket_subscription& subscription) const
	{
		std::string symbol{ subscription.pair_item().to_string(PAIR_SEPARATOR) };

		if (subscription.channel() == websocket_channel::OHLCV)
		{
			return _ohlcvSubscriptionService.shared_lock()->generate_subscription_id(std::move(symbol), subscription.get_ohlcv_interval());
		}

		std::string topic{ get_channel(subscription.channel()) };
		return ::generate_subscription_id(std::move(symbol), std::move(topic));
	}

	void coinbase_websocket_stream::process_price_message(const json_document& json)
	{
		std::string pairName{ json.get<std::string>("product_id") };
		std::string subId{ ::generate_subscription_id(pairName, "ticker") };
		double price{ std::stod(json.get<std::string>("price")) };

		update_price(std::move(subId), price);

		auto lockedOhlcvSubscriptions = _ohlcvSubscriptionService.unique_lock();

		if (lockedOhlcvSubscriptions->is_subscribed(pairName))
		{
			double volume{ std::stod(json.get<std::string>("last_size")) };
			std::time_t time{ to_time_t(json.get<std::string>("time"), "%Y-%m-%dT%T") };

			lockedOhlcvSubscriptions->update_ohlcv(std::move(pairName), time, price, volume);
		}
	}

	void coinbase_websocket_stream::on_message(std::string_view message)
	{
		json_document json{ parse_json(message) };

		std::string messageType{ json.get<std::string>("type") };

		if (messageType == "ticker")
		{
			process_price_message(json);
		}

		/*if (messageType == "snapshot")
		{
			process_order_book_snapshot_message(json, _localOrderBook);
		}
		else if (messageType == "l2update")
		{
			process_order_book_update_message(json, _localOrderBook);
		}*/
	}

	void coinbase_websocket_stream::subscribe(const websocket_subscription& subscription)
	{
		if (subscription.channel() == websocket_channel::OHLCV)
		{
			subscribe(websocket_subscription::create_price_sub(subscription.pair_item()));
			_ohlcvSubscriptionService.unique_lock()->add_subscription(subscription);
			return;
		}

		std::string channelName{ get_channel(subscription.channel()) };
		std::string message{ create_message("subscribe", channelName, subscription.pair_item()) };

		_connection->send_message(message);
	}

	void coinbase_websocket_stream::unsubscribe(const websocket_subscription& subscription)
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