#include "kraken_results.h"
#include "networking/json_wrapper.h"
#include "utils/stringutils.h"

const std::vector<TradablePair> read_tradable_pairs(const std::string& jsonResult)
{
	JsonWrapper json{ jsonResult };
	auto resultObject = json.document()["result"].GetObject();
	std::vector<TradablePair> pairs;
	pairs.reserve(resultObject.MemberCount());

	for (auto it = resultObject.MemberBegin(); it != resultObject.MemberEnd(); ++it)
	{
		std::string name = it->name.GetString();
		std::string wsName = it->value["wsname"].GetString();
		std::vector<std::string> assetSymbols = split(wsName, '/');
		pairs.emplace_back(name, AssetSymbol{ assetSymbols[0] }, AssetSymbol{ assetSymbols[1] });
	}

	return pairs;
}

const OrderBookState read_order_book(const std::string& jsonResult, const TradablePair& pair, int depth)
{
	JsonWrapper json{ jsonResult };

	auto resultObject = json
		.document()["result"]
		.GetObject()
		.FindMember(pair.exchange_identifier().c_str());

	std::vector<OrderBookLevel> levels;
	levels.reserve(depth);

	auto asks = resultObject->value["asks"].GetArray();
	auto bids = resultObject->value["bids"].GetArray();

	for (int i = 0; i < depth; i++)
	{
		auto asks_i = asks[i].GetArray();
		OrderBookEntry askEntry{ std::stod(asks_i[0].GetString()),	std::stod(asks_i[1].GetString()) };

		auto bids_i = bids[i].GetArray();
		OrderBookEntry bidEntry{ std::stod(bids_i[0].GetString()),	std::stod(bids_i[1].GetString()) };

		levels.emplace_back(std::move(askEntry), std::move(bidEntry));
	}

	return OrderBookState{ std::move(levels) };
}

const std::unordered_map<AssetSymbol, double> read_balances(const std::string& jsonResult)
{
	return std::unordered_map<AssetSymbol, double>();
}