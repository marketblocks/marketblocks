#include <chrono>

#include "kraken.h"
#include "networking/json_wrapper.h"
#include "utils/stringutils.h"
#include "utils/containerutils.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"

namespace 
{
	const std::string publicApiKey = "";
	const std::string privateApiKey = "";

	namespace Uris
	{
		static const std::string BASE = "https://api.kraken.com";
		static const std::string TRADABLE_PAIRS = BASE + "/0/public/AssetPairs";
		static const std::string ORDER_BOOK = BASE + "/0/public/Depth";
		static const std::string BALANCE = BASE + "/0/private/Balance";
	}

	const std::string build_pairs_uri(const std::string& resourceUri, const std::vector<TradablePair>& tradablePairs)
	{
		std::string uri = resourceUri + "?pair=";
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

		return OrderBookState{std::move(levels)};
	}

	const std::unordered_map<AssetSymbol, double> read_balances(const std::string& jsonResult)
	{
		return std::unordered_map<AssetSymbol, double>();
	}
}

KrakenApi::KrakenApi(HttpService httpService)
	: _httpService{ std::move(httpService) }, decodedSecret { b64_decode(privateApiKey) }
{
}

std::string KrakenApi::get_nonce() const
{
	return std::to_string(
		std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch())
		.count());
}

std::string KrakenApi::compute_api_sign(const std::string& uriPath, const std::string& urlPostData, const std::string& nonce) const
{
	std::vector<unsigned char> nonce_postData = sha256(nonce + urlPostData);
	std::vector<unsigned char> message{ uriPath.begin(), uriPath.end() };

	message.insert(message.end(), nonce_postData.begin(), nonce_postData.end());

	return b64_encode(hmac_sha512(message, decodedSecret));
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
		std::string uri =Uris::ORDER_BOOK + "?pair=" + pair.exchange_identifier() + "&count=" + std::to_string(depth);
		std::string result = _httpService.get(uri);
		
		orderBooks.emplace(pair, read_order_book(result, pair, depth));
	}

	return orderBooks;
}

const std::unordered_map<TradablePair, double> KrakenApi::get_fees(const std::vector<TradablePair>& tradablePairs) const
{
	return std::unordered_map<TradablePair, double>();
}

const std::unordered_map<AssetSymbol, double> KrakenApi::get_balances() const
{
	std::string nonce = get_nonce();
	std::string postData = "nonce=" + nonce;

	std::string apiSign = compute_api_sign("/0/private/Balance", postData, nonce);

	std::vector<std::pair<std::string, std::string>> headers
	{
		std::make_pair("API-Key", publicApiKey),
		std::make_pair("API-Sign", apiSign),
		std::make_pair("Content-Type", "application/x-www-form-urlencoded; charset=utf-8")
	};

	std::string result = _httpService.post(Uris::BALANCE, postData, headers);

	return std::unordered_map<AssetSymbol, double>();
}

TradeResult KrakenApi::trade(const TradeDescription& description)
{
	return TradeResult::SUCCESS;
}