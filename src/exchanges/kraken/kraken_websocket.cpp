#include "kraken_websocket.h"

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
}

namespace cb
{
	std::string kraken_websocket_stream::get_subscribe_order_book_message(const std::vector<tradable_pair>& tradablePairs) const
	{
		std::string tradablePairList = join_tradable_pairs(tradablePairs);
		std::string message = "{ \"event\": \"subscribe\", \"pair\": [" + std::move(tradablePairList) + "], \"subscription\": { \"name\": \"book\" } }";
		return message;
	}

	void kraken_websocket_stream::on_message(const std::string& message)
	{
		std::cout << message << std::endl;

		json_wrapper json{ message };

		if (json.document().IsObject())
		{
			//std::cout << message << std::endl;
			return;
		}

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

	void kraken_websocket_stream::process_order_book_message(const rapidjson::GenericArray<false, rapidjson::Value>& messageObject)
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

	void kraken_websocket_stream::process_order_book_object(const std::string& pair, const rapidjson::GenericObject<false, rapidjson::Value>& object)
	{
		for (auto it = object.MemberBegin(); it < object.MemberEnd(); it++)
		{
			std::string entrySideString = it->name.GetString();

			order_book_side side = order_book_side::ASK;

			if (entrySideString.contains("b"))
			{
				side = order_book_side::BID;
			}
			else if (!entrySideString.contains("a"))
			{
				continue;
			}

			auto entriesArray = it->value.GetArray();

			bool replace = false;
			std::string priceToReplace;

			for (auto it = entriesArray.Begin(); it < entriesArray.End(); it++)
			{
				auto entryArray = it->GetArray();
				std::string price = entryArray[0].GetString();
				order_book_entry entry
				{
					side,
					std::stod(price),
					std::stod(entryArray[1].GetString()),
					std::stod(entryArray[2].GetString())
				};

				if (entry.volume() == 0.0)
				{
					replace = true;
					priceToReplace = std::move(price);
					continue;
				}

				auto cacheIt = _orderBookCaches.find(pair);

				if (replace)
				{
					cacheIt->second.replace(priceToReplace, std::move(price), std::move(entry));
				}
				else
				{
					cacheIt->second.cache(std::move(price), std::move(entry));
				}
			}
		}
	}
}