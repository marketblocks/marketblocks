#include "kraken.h"
#include "networking/json_wrapper.h"
#include "utils/stringutils.h"
#include "utils/vectorutils.h"

namespace 
{
	const std::string build_price_data_uri(const std::vector<TradablePair>& tradablePairs)
	{
		std::string uri = "https://api.kraken.com/0/public/Ticker?pair=";

		auto append_symbol = [&uri](const TradablePair& tradablePair)
		{
			uri += tradablePair.asset() + tradablePair.price_unit();
		};

		for (int i = 0; i < tradablePairs.size() - 1; ++i)
		{
			append_symbol(tradablePairs[i]);
			uri += ',';
		}

		append_symbol(tradablePairs[tradablePairs.size() - 1]);

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

	const std::unordered_map<TradablePair, PriceData> read_price_data(const std::string& jsonResult, const std::vector<TradablePair>& tradablePairs)
	{
		JsonWrapper json{ jsonResult };
		auto resultObject = json.document()["result"].GetObject();
		std::unordered_map<TradablePair, PriceData> priceDataMap;
		priceDataMap.reserve(resultObject.MemberCount());

		std::unordered_map<std::string, TradablePair> tradablePairLookup = to_unordered_map<std::string, TradablePair>(
			tradablePairs,
			[](const TradablePair& pair) { return pair.exchange_identifier(); },
			[](const TradablePair& pair) { return pair; });

		for (auto it = resultObject.MemberBegin(); it != resultObject.MemberEnd(); ++it)
		{
			std::string name = it->name.GetString();
			double askPrice = std::stod(it->value["a"].GetArray()[0].GetString());
			double bidPrice = std::stod(it->value["b"].GetArray()[0].GetString());
			PriceData data{ bidPrice, askPrice };

			priceDataMap.emplace(tradablePairLookup[name], std::move(data));
		}

		return priceDataMap;
	}
}

KrakenApi::KrakenApi(HttpService httpService)
	: _httpService{ std::move(httpService) }
{
}

const std::vector<TradablePair> KrakenApi::get_tradable_pairs() const
{
	std::string result = _httpService.get("https://api.kraken.com/0/public/AssetPairs");
	return read_tradable_pairs(result);
}

const std::unordered_map<TradablePair, PriceData> KrakenApi::get_price_data(const std::vector<TradablePair>& tradablePairs) const
{
	std::string uri = build_price_data_uri(tradablePairs);
	std::string result = _httpService.get(uri);
	return read_price_data(result, tradablePairs);
}

double KrakenApi::get_fee(const TradablePair& tradablePair) const
{
	return 0.26;
}

const std::unordered_map<TradablePair, double> KrakenApi::get_fees(const std::vector<TradablePair>& tradablePairs) const
{
	return std::unordered_map<TradablePair, double>();
}

const std::unordered_map<std::string, double> KrakenApi::get_all_balances() const
{
	return std::unordered_map<std::string, double>();
}

double KrakenApi::get_balance(const std::string& tickerId) const
{
	return 0.0;
}

void KrakenApi::trade(const TradeDescription& description)
{

}
