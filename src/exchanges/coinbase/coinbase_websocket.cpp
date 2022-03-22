#include "coinbase_websocket.h"

#include "coinbase.h"
#include "common/utils/containerutils.h"

namespace
{
	using namespace cb;

	tradable_pair get_pair_from_id(std::string_view productId)
	{
		std::vector<std::string> parts = split(productId, '-');
		return tradable_pair{ parts[0], parts[1] };
	}

	std::pair<std::string, cb::order_book_entry> construct_order_book_map_pair(const cb::json_element& json)
	{
		std::string price{ json.get<std::string>(0) };
		order_book_entry entry
		{
			std::stod(price),
			std::stod(json.get<std::string>(1))
		};

		return std::make_pair(std::move(price), std::move(entry));
	}

	std::string create_message(std::string_view type, std::string channel, const std::vector<tradable_pair>& pairs)
	{
		std::vector<std::string> tradablePairList{ to_vector<std::string>(pairs, [](const tradable_pair& pair) { return internal::to_exchange_id(pair); }) };

		return json_writer{}
			.add("type", type)
			.add("product_ids", std::move(tradablePairList))
			.add("channels", std::vector<std::string>{ channel })
			.to_string();
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
	}
}

namespace cb::internal
{
	void coinbase_websocket_stream::on_open()
	{
		logger::instance().info("Successfully connected to Coinbase websocket feed");
	}

	void coinbase_websocket_stream::on_close(std::string_view reason)
	{

	}

	void coinbase_websocket_stream::on_fail(std::string_view reason)
	{

	}

	void coinbase_websocket_stream::on_message(std::string_view message)
	{
		json_document json{ parse_json(message) };

		std::string messageType{ json.get<std::string>("type") };

		if (messageType == "snapshot")
		{
			process_order_book_snapshot_message(json, _localOrderBook);
		}
		else if (messageType == "l2update")
		{
			process_order_book_update_message(json, _localOrderBook);
		}
	}

	std::string coinbase_websocket_stream::get_order_book_subscription_message(const std::vector<tradable_pair>& tradablePairs) const
	{
		return create_message("subscribe", "level2", tradablePairs);
	}

	std::string coinbase_websocket_stream::get_order_book_unsubscription_message(const std::vector<tradable_pair>& tradablePairs) const
	{
		return create_message("unsubscribe", "level2", tradablePairs);
	}
}