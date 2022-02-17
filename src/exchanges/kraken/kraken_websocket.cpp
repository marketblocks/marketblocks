#include "kraken_websocket.h"
#include "logging/logger.h"
#include "common/utils/containerutils.h"

namespace
{
	std::string join_tradable_pairs(const std::vector<cb::tradable_pair>& tradablePairs)
	{
		std::string result;

		for (auto& pair : tradablePairs)
		{
			result += "\"" + pair.iso_4217_a3() + "\"" + ",";
		}

		result.erase(--result.end());

		return result;
	}

	cb::exchange_status to_exchange_status(const std::string& statusString)
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

	cb::cache_entry get_order_book_cache_entry(cb::order_book_side side, const cb::json_element& json)
	{
		std::string price = json.element(0).get<std::string>();
		cb::order_book_entry entry
		{
			side,
			std::stod(price),
			std::stod(json.element(1).get<std::string>()),
			std::stod(json.element(2).get<std::string>())
		};

		return cb::cache_entry
		{
			std::move(price),
			std::move(entry)
		};
	}
}

namespace cb
{
	kraken_websocket_stream::kraken_websocket_stream(std::shared_ptr<websocket_client> websocketClient)
		: websocket_stream{ websocketClient }
	{}

	void kraken_websocket_stream::process_event_message(const json_document& json)
	{
		std::string eventName{ json.get<std::string>("event") };

		if (eventName == "systemStatus")
		{
			log_status_change(to_exchange_status(json.get<std::string>("status")));
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
			std::string pair = json.element(3).get<std::string>();
			json_element entryObject = json.element(1);

			if (is_order_book_subscribed(pair))
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
			std::string pair = json.element(4).get<std::string>();
			process_order_book_object(pair, json.element(1));
			process_order_book_object(pair, json.element(2));
		}
	}

	void kraken_websocket_stream::process_order_book_initialisation(const std::string& pair, const json_element& json)
	{
		json_element asks = json.element("as");
		json_element bids = json.element("bs");

		int depth = asks.size();

		std::vector<cache_entry> askEntries;
		askEntries.reserve(depth);

		std::vector<cache_entry> bidEntries;
		bidEntries.reserve(depth);

		for (int i = 0; i < depth; ++i)
		{
			askEntries.emplace_back(get_order_book_cache_entry(order_book_side::ASK, asks.element(i)));
			bidEntries.emplace_back(get_order_book_cache_entry(order_book_side::BID, bids.element(i)));
		}

		initialise_order_book_cache(pair, std::move(askEntries), std::move(bidEntries));
	}

	void kraken_websocket_stream::process_order_book_object(const std::string& pair, const json_element& json)
	{
		json_iterator first = json.begin();
		std::string entrySideId = first.key();
		json_element element = first.value();

		order_book_side side = get_order_book_side(entrySideId); 
		bool replace = false;
		std::string priceToReplace;

		for (auto entryIterator = element.begin(); entryIterator != element.end(); ++entryIterator)
		{
			json_element entryJson = entryIterator.value();
			cache_entry cacheEntry = get_order_book_cache_entry(side, entryJson);

			if (cacheEntry.entry.volume() == 0.0)
			{
				replace = true;
				priceToReplace = std::move(cacheEntry.price);
				continue;
			}

			if (replace)
			{
				cache_replacement replacement
				{
					priceToReplace,
					std::move(cacheEntry.price),
					std::move(cacheEntry.entry)
				};

				replace_in_order_book_cache(pair, std::move(replacement));
			}
			else
			{
				update_order_book_cache(pair, cacheEntry);
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

	void kraken_websocket_stream::on_close(const std::string& reason)
	{
		logger::instance().info("Kraken websocket connection closed: {}", reason);
	}

	void kraken_websocket_stream::on_fail(const std::string& reason)
	{
		logger::instance().error("Kraken websocket connection failed: {}", reason);
	}

	void kraken_websocket_stream::on_message(const std::string& message)
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

	void kraken_websocket_stream::subscribe_order_book(const std::vector<tradable_pair>& tradablePairs)
	{
		std::string tradablePairList = join_tradable_pairs(tradablePairs);
		std::string message = "{ \"event\": \"subscribe\", \"pair\": [" + std::move(tradablePairList) + "], \"subscription\": { \"name\": \"book\" } }";

		send_message(message);
	}
}