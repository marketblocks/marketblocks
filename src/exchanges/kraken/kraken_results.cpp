#include "kraken_results.h"
#include "common/file/json_wrapper.h"
#include "common/utils/stringutils.h"

namespace cb::internal
{
	exchange_status read_system_status(const std::string& jsonResult)
	{
		json_wrapper json{ jsonResult };
		auto resultObject = json.document()["result"].GetObject();

		std::string status_string = resultObject["status"].GetString();

		if (status_string == "online")
		{
			return exchange_status::ONLINE;
		}
		else if (status_string == "cancel_only")
		{
			return exchange_status::CANCEL_ONLY;
		}
		else if (status_string == "post_only")
		{
			return exchange_status::POST_ONLY;
		}
		else
		{
			return exchange_status::MAINTENANCE;
		}
	}

	const std::vector<tradable_pair> read_tradable_pairs(const std::string& jsonResult)
	{
		json_wrapper json{ jsonResult };
		auto resultObject = json.document()["result"].GetObject();
		std::vector<tradable_pair> pairs;
		pairs.reserve(resultObject.MemberCount());

		for (auto it = resultObject.MemberBegin(); it != resultObject.MemberEnd(); ++it)
		{
			std::string name = it->name.GetString();
			std::string wsName = it->value["wsname"].GetString();
			std::vector<std::string> assetSymbols = split(wsName, '/');
			pairs.emplace_back(name, asset_symbol{ assetSymbols[0] }, asset_symbol{ assetSymbols[1] });
		}

		return pairs;
	}

	const order_book_state read_order_book(const std::string& jsonResult, const tradable_pair& pair, int depth)
	{
		json_wrapper json{ jsonResult };

		auto resultObject = json
			.document()["result"]
			.GetObject()
			.FindMember(pair.exchange_identifier().c_str());

		std::vector<order_book_level> levels;
		levels.reserve(depth);

		auto asks = resultObject->value["asks"].GetArray();
		auto bids = resultObject->value["bids"].GetArray();

		for (int i = 0; i < depth; i++)
		{
			auto asks_i = asks[i].GetArray();
			order_book_entry askEntry
			{
				order_book_side::ASK,
				std::stod(asks_i[0].GetString()),
				std::stod(asks_i[1].GetString()),
				asks_i[2].GetDouble()
			};

			auto bids_i = bids[i].GetArray();
			order_book_entry bidEntry
			{
				order_book_side::BID,
				std::stod(bids_i[0].GetString()),
				std::stod(bids_i[1].GetString()),
				bids_i[2].GetDouble()
			};

			levels.emplace_back(std::move(askEntry), std::move(bidEntry));
		}

		return order_book_state{ std::move(levels) };
	}

	const std::unordered_map<asset_symbol, double> read_balances(const std::string& jsonResult)
	{
		return std::unordered_map<asset_symbol, double>();
	}
}