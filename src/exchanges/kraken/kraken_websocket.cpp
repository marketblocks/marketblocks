#include "kraken_websocket.h"

namespace
{
	std::string join_tradable_pairs(const std::vector<TradablePair>& tradablePairs)
	{
		std::string result;

		for (auto& pair : tradablePairs)
		{
			result += "\"" + pair.iso_4217_a3() + "\"" + ",";
		}

		result.erase(--result.end());

		return result;
	}
}

void KrakenWebsocketStream::onMessage(const std::string& message)
{
	JsonWrapper json{ message };
	
	if (json.document().IsObject())
	{
		//std::cout << message << std::endl;
		return;
	}
	//std::cout << message << std::endl;
	auto messageArray = json.document().GetArray();

	std::string channelName = messageArray[messageArray.Size() - 2].GetString();
	if (channelName.contains("book"))
	{
		process_order_book_message(messageArray);
	}
	else
	{

	}
}

void KrakenWebsocketStream::process_order_book_message(const rapidjson::GenericArray<false, rapidjson::Value>& messageObject)
{
	if (messageObject.Size() == 4)
	{
		std::string pair = messageObject[3].GetString();
		auto entryObject = messageObject[1].GetObjectA();

		process_order_book_object(pair, entryObject);

		// could be a, as, b, bs

		//std::cout << "message size: 4" << std::endl;
	}
	else
	{
		std::string pair = messageObject[4].GetString();
		auto entryObject1 = messageObject[1].GetObjectA();
		auto entryObject2 = messageObject[2].GetObjectA();

		process_order_book_object(pair, entryObject1);
		process_order_book_object(pair, entryObject2);

		//std::cout << "message size: 5" << std::endl;
	}
}

void KrakenWebsocketStream::process_order_book_object(const std::string& pair, const rapidjson::GenericObject<false, rapidjson::Value>& object)
{
	for (auto it = object.MemberBegin(); it < object.MemberEnd(); it++)
	{
		std::string entrySideString = it->name.GetString();

		OrderBookSide side = OrderBookSide::ASK;

		if (entrySideString.contains("b"))
		{
			side = OrderBookSide::BID;
		}
		else if (!entrySideString.contains("a"))
		{
			continue;
		}

		auto entriesArray = it->value.GetArray();

		for (auto it = entriesArray.Begin(); it < entriesArray.End(); it++)
		{
			auto entryArray = it->GetArray();
			std::string price = entryArray[0].GetString();
			OrderBookEntry entry
			{
				side,
				std::stod(price),
				std::stod(entryArray[1].GetString()),
				std::stod(entryArray[2].GetString())
			};
			_orderBookCaches.at(pair).cache(price, entry);
		}
	}
}

void KrakenWebsocketStream::subscribe_order_book(const std::vector<TradablePair>& tradablePairs)
{
	std::string tradablePairList = join_tradable_pairs(tradablePairs);
	std::string message = "{ \"event\": \"subscribe\", \"pair\": [" + tradablePairList + "], \"subscription\": { \"name\": \"book\" } }";
	
	for (auto& pair : tradablePairs)
	{
		_orderBookCaches.emplace(pair.iso_4217_a3(), OrderBookCache{10});
	}

	send_message(message);
}

OrderBookState KrakenWebsocketStream::get_current_order_book(const TradablePair& tradablePair) const
{
	return _orderBookCaches.at(tradablePair.iso_4217_a3()).snapshot();
}