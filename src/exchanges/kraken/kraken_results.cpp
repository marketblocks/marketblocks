#include "kraken_results.h"
#include "common/file/json.h"
#include "common/utils/stringutils.h"

namespace cb::internal
{
	exchange_status read_system_status(const std::string& jsonResult)
	{
		json_document jsonDocument = parse_json(jsonResult);
		json_element resultElement = jsonDocument.get<json_element>("result");

		std::string status_string = resultElement.get<std::string>("status");

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
		json_document jsonDocument = parse_json(jsonResult);
		json_element resultElement = jsonDocument.get<json_element>("result");

		std::vector<tradable_pair> pairs;
		pairs.reserve(resultElement.size());

		for (auto it = resultElement.begin(); it != resultElement.end(); ++it)
		{
			std::string name = it.key();
			std::string wsName = it.value().get<std::string>("wsname");
			std::vector<std::string> assetSymbols = split(wsName, '/');
			pairs.emplace_back(name, asset_symbol{ assetSymbols[0] }, asset_symbol{ assetSymbols[1] });
		}

		return pairs;
	}

	const order_book_state read_order_book(const std::string& jsonResult, const tradable_pair& pair, int depth)
	{
		json_document jsonDocument = parse_json(jsonResult);
		json_element resultElement = jsonDocument
			.get<json_element>("result")
			.get<json_element>(pair.exchange_identifier());

		std::vector<order_book_level> levels;
		levels.reserve(depth);

		auto asks = resultElement.get<json_element>("asks");
		auto bids = resultElement.get<json_element>("bids");

		for (int i = 0; i < depth; i++)
		{
			json_element asks_i = asks.element(i);
			order_book_entry askEntry
			{
				order_book_side::ASK,
				std::stod(asks_i.element(0).get<std::string>()),
				std::stod(asks_i.element(1).get<std::string>()),
				asks_i.element(2).get<double>()
			};

			json_element bids_i = bids.element(i);
			order_book_entry bidEntry
			{
				order_book_side::BID,
				std::stod(bids_i.element(0).get<std::string>()),
				std::stod(bids_i.element(1).get<std::string>()),
				bids_i.element(2).get<double>()
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