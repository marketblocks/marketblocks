#include "kraken_websocket.h"
#include "logging/logger.h"
#include "common/utils/containerutils.h"
#include "common/utils/stringutils.h"
#include "common/exceptions/mb_exception.h"

namespace
{
	using namespace mb;

	std::string create_message(std::string_view eventName, std::string_view channel, const std::vector<tradable_pair>& tradablePairs)
	{
		std::vector<std::string> tradablePairList{ to_vector<std::string>(tradablePairs, [](const tradable_pair& pair) { return pair.to_string('/'); })};

		return json_writer{}
			.add("event", eventName)
			.add("pair", std::move(tradablePairList))
			.add("subscription", json_writer{}.add("name", channel).to_json())
			.to_string();
	}

	order_book_side get_order_book_side(const std::string sideId)
	{
		return sideId.contains("a") 
			? order_book_side::ASK
			: order_book_side::BID;
	}

	std::pair<std::string, order_book_entry> construct_order_book_map_pair(const json_element& json)
	{
		std::string price{ json.element(0).get<std::string>() };
		order_book_entry entry
		{
			std::stod(price),
			std::stod(json.element(1).get<std::string>())
		};

		return std::make_pair<std::string, order_book_entry>(std::move(price), std::move(entry));
	}

	order_book_cache_entry create_order_book_cache_entry(order_book_side side, const json_element& json)
	{
		std::pair<std::string, order_book_entry> pair{ construct_order_book_map_pair(json) };
		return order_book_cache_entry
		{
			side,
			std::move(pair.first),
			std::move(pair.second)
		};
	}

	void process_order_book_initialisation(const tradable_pair& pair, local_order_book& localOrderBook, const json_element& json)
	{
		json_element asks = json.element("as");
		json_element bids = json.element("bs");

		int depth = asks.size();

		ask_map askMap;
		bid_map bidMap;

		for (int i = 0; i < depth; ++i)
		{
			askMap.emplace(construct_order_book_map_pair(asks.element(i)));
			bidMap.emplace(construct_order_book_map_pair(bids.element(i)));
		}

		localOrderBook.initialise_book(pair, std::move(askMap), std::move(bidMap), depth);
		logger::instance().info("Kraken: initialised order book for {}", pair.to_string('/'));
	}

	void process_order_book_object(const tradable_pair& pair, local_order_book& localOrderBook, const json_element& json)
	{
		json_iterator first = json.begin();
		std::string entrySideId = first.key();
		json_element element = first.value();

		order_book_side side = get_order_book_side(entrySideId);
		bool replace = false;
		std::string priceToReplace;

		for (auto entryIterator = element.begin(); entryIterator != element.end(); ++entryIterator)
		{
			json_element entryJson{ entryIterator.value() };
			order_book_cache_entry cacheEntry{ create_order_book_cache_entry(side, entryJson) };

			if (cacheEntry.entry.volume() == 0.0)
			{
				replace = true;
				priceToReplace = std::move(cacheEntry.price);
				continue;
			}

			if (replace)
			{
				localOrderBook.replace_in_book(pair, std::move(priceToReplace), std::move(cacheEntry));
			}
			else
			{
				localOrderBook.update_book(pair, std::move(cacheEntry));
			}
		}
	}

	void process_order_book_message(const json_document& json, local_order_book& localOrderBook)
	{
		if (json.size() == 4)
		{
			tradable_pair pair{ parse_tradable_pair(json.element(3).get<std::string>(), '/')};
			json_element entryObject = json.element(1);

			if (localOrderBook.is_subscribed(pair))
			{
				process_order_book_object(pair, localOrderBook, std::move(entryObject));
			}
			else
			{
				process_order_book_initialisation(pair, localOrderBook, std::move(entryObject));
			}
		}
		else
		{
			tradable_pair pair{ parse_tradable_pair(json.element(4).get<std::string>(), '/')};
			process_order_book_object(pair, localOrderBook, json.element(1));
			process_order_book_object(pair, localOrderBook, json.element(2));
		}
	}
}

namespace mb::internal
{
	void kraken_websocket_stream::on_open()
	{
		logger::instance().info("Successfully connected Kraken websocket feed");
	}

	void kraken_websocket_stream::on_close(std::error_code reason)
	{
		logger::instance().info("Kraken websocket connection closed: {}", reason.message());
	}

	void kraken_websocket_stream::on_fail(std::error_code reason)
	{
		logger::instance().error("Kraken websocket connection failed: {}", reason.message());
	}

	void kraken_websocket_stream::on_message(std::string_view message)
	{
		json_document json = parse_json(message);
		json_value_type type = json.type();

		if (json.type() == json_value_type::ARRAY)
		{
			std::string channelName = json.element(json.size() - 2).get<std::string>();

			if (channelName.contains("book"))
			{
				process_order_book_message(json, _localOrderBook);
			}
		}
	}

	std::string kraken_websocket_stream::get_order_book_subscription_message(const std::vector<tradable_pair>& tradablePairs) const
	{
		return create_message("subscribe", "book", tradablePairs);
	}

	std::string kraken_websocket_stream::get_order_book_unsubscription_message(const std::vector<tradable_pair>& tradablePairs) const
	{
		return create_message("unsubscribe", "book", tradablePairs);
	}
}