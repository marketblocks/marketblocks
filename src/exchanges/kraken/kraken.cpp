#include "kraken.h"
#include "networking/json_wrapper.h"
#include "utils/stringutils.h"
#include "utils/containerutils.h"

namespace 
{
	namespace Uris
	{
		static const std::string BASE = "https://api.kraken.com/0/public/";
		static const std::string TRADABLE_PAIRS = BASE + "AssetPairs";
		static const std::string ORDER_BOOK = BASE + "Depth?pair=";
	}

	const std::string build_pairs_uri(const std::string& resourceUri, const std::vector<TradablePair>& tradablePairs)
	{
		std::string uri = resourceUri;
		int count = tradablePairs.size();
		
		for (int i = 0; i < count - 1; ++i)
		{
			uri += tradablePairs[i].exchange_identifier();
			uri += ',';
		}

		uri += tradablePairs[count - 1].exchange_identifier();

		return uri;
	}

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
			pairs.emplace_back(name, assetSymbols[0], assetSymbols[1]);
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

		return OrderBookState{std::move(levels)};
	}
}

KrakenApi::KrakenApi(HttpService httpService)
	: _httpService{ std::move(httpService) }
{
}

const std::vector<TradablePair> KrakenApi::get_tradable_pairs() const
{
	std::string result = _httpService.get(Uris::TRADABLE_PAIRS);
	return read_tradable_pairs(result);
}

const std::unordered_map<TradablePair, OrderBookState> KrakenApi::get_order_book(const std::vector<TradablePair>& tradablePairs, int depth) const
{
	std::unordered_map<TradablePair, OrderBookState> orderBooks;
	orderBooks.reserve(depth);

	for (auto& pair : tradablePairs)
	{
		std::string uri =Uris::ORDER_BOOK + pair.exchange_identifier() + "&count=" + std::to_string(depth);
		std::string result = _httpService.get(uri);
		
		orderBooks.emplace(pair, read_order_book(result, pair, depth));
	}

	return orderBooks;
}

const std::unordered_map<TradablePair, double> KrakenApi::get_fees(const std::vector<TradablePair>& tradablePairs) const
{
	return std::unordered_map<TradablePair, double>();
}

const std::unordered_map<std::string, double> KrakenApi::get_balances() const
{
	return std::unordered_map<std::string, double>();
}

void KrakenApi::trade(const TradeDescription& description)
{

}