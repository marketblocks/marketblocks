#include "kraken_websocket.h"
#include "logging/logger.h"
#include "common/utils/containerutils.h"
#include "common/utils/stringutils.h"
#include "common/exceptions/cb_exception.h"

namespace
{
	std::string join_tradable_pairs(const std::vector<cb::tradable_pair>& tradablePairs)
	{
		std::string result;

		for (auto& pair : tradablePairs)
		{
			result += "\"" + pair.to_standard_string() + "\"" + ",";
		}

		result.erase(--result.end());

		return result;
	}

	cb::exchange_status to_exchange_status(std::string_view statusString)
	{
		if (statusString == "online")
			return cb::exchange_status::ONLINE;
		else if (statusString == "maintenance")
			return cb::exchange_status::MAINTENANCE;
		else if (statusString == "cancel_only")
			return cb::exchange_status::CANCEL_ONLY;
		else if (statusString == "limit_only")
			return cb::exchange_status::LIMIT_ONLY;
		else if (statusString == "post_only")
			return cb::exchange_status::POST_ONLY;
		else
			return cb::exchange_status::OFFLINE;
	}

	cb::order_book_side get_order_book_side(const std::string sideId)
	{
		return sideId.contains("a") 
			? cb::order_book_side::ASK
			: cb::order_book_side::BID;
	}

	std::pair<std::string, cb::order_book_entry> construct_order_book_map_pair(const cb::json_element& json)
	{
		std::string price{ json.element(0).get<std::string>() };
		cb::order_book_entry entry
		{
			std::stod(price),
			std::stod(json.element(1).get<std::string>())
		};

		return std::make_pair<std::string, cb::order_book_entry>(std::move(price), std::move(entry));
	}

	cb::order_book_cache_entry create_order_book_cache_entry(cb::order_book_side side, const cb::json_element& json)
	{
		std::pair<std::string, cb::order_book_entry> pair{ construct_order_book_map_pair(json) };
		return cb::order_book_cache_entry
		{
			side,
			std::move(pair.first),
			std::move(pair.second)
		};
	}

	cb::tradable_pair parse_tradable_pair(const std::string& pairName)
	{
		std::vector<std::string> assets{ cb::split(pairName, '/') };
		return cb::tradable_pair{ std::move(assets[0]), std::move(assets[1]) };
	}
}

namespace cb::internal
{
	void kraken_websocket_stream::process_event_message(const json_document& json)
	{
		std::string eventName{ json.get<std::string>("event") };

		if (eventName == "systemStatus")
		{
			_currentStatus = to_exchange_status(json.get<std::string>("status"));
		}
		else if (eventName == "error")
		{
			throw cb_exception{ json.get<std::string>("errorMessage") };
		}
		else if (eventName == "subscriptionStatus")
		{
			std::string pair = json.get<std::string>("pair");
			std::string channelName = json.element("subscription").get<std::string>("name");
			bool subscribed = json.get<std::string>("status") == "subscribed";

			if (subscribed)
			{
				logger::instance().info("Successfully suscribed to channel '{0}' for pair {1}", channelName, pair);
			}
			else
			{
				std::string error = json.get<std::string>("errorMessage");
				logger::instance().error("Failed to subscribe to channel '{0}' for pair {1}: {2}", channelName, pair, error);
			}
		}
	}

	void kraken_websocket_stream::process_order_book_message(const json_document& json)
	{
		if (json.size() == 4)
		{
			tradable_pair pair{ parse_tradable_pair(json.element(3).get<std::string>()) };
			json_element entryObject = json.element(1);

			if (_localOrderBook.is_subscribed(pair))
			{
				process_order_book_object(pair, std::move(entryObject));
			}
			else
			{
				process_order_book_initialisation(pair, std::move(entryObject));
			}
		}
		else
		{
			tradable_pair pair{ parse_tradable_pair(json.element(4).get<std::string>()) };
			process_order_book_object(pair, json.element(1));
			process_order_book_object(pair, json.element(2));
		}
	}

	void kraken_websocket_stream::process_order_book_initialisation(const tradable_pair& pair, const json_element& json)
	{
		json_element asks = json.element("as");
		json_element bids = json.element("bs");

		int depth = 10;

		ask_map askMap;
		bid_map bidMap;

		for (int i = 0; i < depth; ++i)
		{
			askMap.emplace(construct_order_book_map_pair(asks.element(i)));
			bidMap.emplace(construct_order_book_map_pair(bids.element(i)));
		}

		_localOrderBook.initialise_book(pair, std::move(askMap), std::move(bidMap), depth);
	}

	void kraken_websocket_stream::process_order_book_object(const tradable_pair& pair, const json_element& json)
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
				_localOrderBook.replace_in_book(pair, std::move(priceToReplace), std::move(cacheEntry));
			}
			else
			{
				_localOrderBook.update_book(pair, std::move(cacheEntry));
			}
		}
	}

	void kraken_websocket_stream::process_update_message(const json_document& json)
	{
		std::string channelName = json.element(json.size() - 2).get<std::string>();

		if (channelName.contains("book"))
		{
			process_order_book_message(json);
		}
	}

	void kraken_websocket_stream::on_open()
	{
		logger::instance().info("Kraken websocket connection opened");
	}

	void kraken_websocket_stream::on_close(std::string_view reason)
	{
		logger::instance().info("Kraken websocket connection closed: {}", reason);
	}

	void kraken_websocket_stream::on_fail(std::string_view reason)
	{
		logger::instance().error("Kraken websocket connection failed: {}", reason);
	}

	void kraken_websocket_stream::on_message(std::string_view message)
	{
		json_document json = parse_json(message);
		json_value_type type = json.type();

		if (json.type() == json_value_type::OBJECT)
		{
			process_event_message(json);
		}
		else
		{
			process_update_message(json);
		}
	}

	std::string kraken_websocket_stream::get_order_book_subscription_message(const std::vector<tradable_pair>& tradablePairs) const
	{
		std::vector<std::string> tradablePairList{ to_vector<std::string>(tradablePairs, [](const tradable_pair& pair) { return pair.to_standard_string(); }) };

		return json_writer{}
			.add("event", "subscribe")
			.add("pair", std::move(tradablePairList))
			.add("subscription", json_writer{}.add("name", "book"))
			.to_string();
	}
}