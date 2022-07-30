#include "binance_websocket.h"
#include "exchanges/exchange_ids.h"
#include "common/utils/containerutils.h"
#include "common/utils/timeutils.h"
#include "logging/logger.h"

namespace
{
	using namespace mb;

	std::string create_message(std::string method, std::string channel, const std::vector<tradable_pair>& pairs)
	{
		std::vector<std::string> params{ to_vector<std::string>(pairs, [&channel](const tradable_pair& pair)
			{
				std::string pairName{ pair.to_string() };
				to_lower(pairName);
				return std::move(pairName) + "@" + channel;
			}) };

		json_writer json;
		json.add("method", std::move(method));
		json.add("params", std::move(params));
		json.add("id", 1);

		return json.to_string();
	}

	std::string get_kline_channel_name(std::string interval)
	{
		return "kline_" + std::move(interval);
	}

	std::string get_channel_name(const websocket_subscription& subscription)
	{
		switch (subscription.channel())
		{
		case websocket_channel::TRADE:
			return "trade";
		case websocket_channel::OHLCV:
			return get_kline_channel_name(to_string(subscription.get_ohlcv_interval()));
		case websocket_channel::ORDER_BOOK:
			return "depth@100ms";
		default:
			throw mb_exception{ "Websocket channel not supported on Binance" };
		}
	}
}

namespace mb::internal
{
	binance_websocket_stream::binance_websocket_stream(
		std::unique_ptr<websocket_connection_factory> connectionFactory,
		std::unique_ptr<market_api> marketApi)
		: 
		exchange_websocket_stream
		{ 
			exchange_ids::BINANCE, 
			"wss://stream.binance.com:9443/ws",
			'\0',
			std::move(connectionFactory) 
		},
		_marketApi{ std::move(marketApi) }
	{}

	void binance_websocket_stream::process_trade_message(const json_document& json)
	{
		std::string symbol{ json.get<std::string>("s") };

		double price{ std::stod(json.get<std::string>("p")) };
		double volume{ std::stod(json.get<std::string>("q")) };
		std::time_t time{ json.get<std::time_t>("T") / 1000 };

		update_trade(std::move(symbol), trade_update{time, price, volume});
	}

	void binance_websocket_stream::process_ohlcv_message(const json_document& json)
	{
		std::string symbol{ json.get<std::string>("s") };
		json_element klineElement{ json.element("k") };
		std::string interval{ klineElement.get<std::string>("i") };

		ohlcv_data ohlcv
		{
			klineElement.get<std::time_t>("t") / 1000,
			std::stod(klineElement.get<std::string>("o")),
			std::stod(klineElement.get<std::string>("h")),
			std::stod(klineElement.get<std::string>("l")),
			std::stod(klineElement.get<std::string>("c")),
			std::stod(klineElement.get<std::string>("v"))
		};

		update_ohlcv(std::move(symbol), parse_ohlcv_interval(interval), std::move(ohlcv));
	}

	void binance_websocket_stream::process_order_book_message(const json_document& json)
	{
		std::string symbol{ json.get<std::string>("s") };

		if (!contains(_orderBookIds, symbol))
		{
			order_book_state snapshot{ _marketApi->get_order_book(_pairLookup.at(symbol), 100) };
			_orderBookIds[symbol] = snapshot.time_stamp();
			initialise_order_book(symbol, from_snapshot(snapshot));
			return;
		}

		std::time_t lastUpdate{ _orderBookIds[symbol] };
		std::time_t finalUpdateId{ json.get<std::time_t>("u") };

		if (finalUpdateId <= lastUpdate)
		{
			return;
		}

		process_order_book_element(order_book_side::ASK, symbol, finalUpdateId, json.element("a"));
		process_order_book_element(order_book_side::BID, symbol, finalUpdateId, json.element("b"));

		_orderBookIds[symbol] = finalUpdateId;
	}

	void binance_websocket_stream::process_order_book_element(order_book_side side, std::string_view pair, std::time_t id, const json_element& element)
	{
		for (auto it = element.begin(); it != element.end(); ++it)
		{
			json_element entryElement{ it.value() };
			update_order_book(std::string{ pair }, id, order_book_entry
				{
					std::stod(entryElement.get<std::string>(0)),
					std::stod(entryElement.get<std::string>(1)),
					side
				});
		}
	}

	void binance_websocket_stream::on_message(std::string_view message)
	{
		json_document json{ parse_json(message) };

		if (json.has_member("msg"))
		{
			logger::instance().error("Binance websocket: {}", json.get<std::string>("msg"));
			return;
		}

		if (json.has_member("e"))
		{
			std::string channel{ json.get<std::string>("e") };

			if (channel == "trade")
			{
				process_trade_message(json);
				return;
			}
			else if (channel == "kline")
			{
				process_ohlcv_message(json);
				return;
			}
			else if (channel == "depthUpdate")
			{
				process_order_book_message(json);
				return;
			}
		}
	}

	void binance_websocket_stream::send_subscribe(const websocket_subscription& subscription)
	{
		for (auto& pair : subscription.pair_item())
		{
			_pairLookup.try_emplace(pair.to_string(), pair);
		}

		std::string message{ create_message("SUBSCRIBE", get_channel_name(subscription), subscription.pair_item()) };
		_connection->send_message(std::move(message));
	}

	void binance_websocket_stream::send_unsubscribe(const websocket_subscription& subscription)
	{
		std::string message{ create_message("UNSUBSCRIBE", get_channel_name(subscription), subscription.pair_item()) };
		_connection->send_message(std::move(message));
	}
}