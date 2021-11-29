#include "kraken.h"
#include "..\..\networking\json.h"
#include "..\..\utils\stringutils.h"
#include "..\..\utils\vectorutils.h"

KrakenMarketData::KrakenMarketData()
	: httpService{}
{
}

const std::string KrakenMarketData::build_price_data_uri(const std::vector<TradablePair>& tradablePairs) const
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

const std::vector<TradablePair> KrakenMarketData::read_tradable_pairs(const std::string& jsonResult) const
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

const std::unordered_map<TradablePair, PriceData> KrakenMarketData::read_price_data(const std::string& jsonResult, const std::vector<TradablePair>& tradablePairs) const
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
		PriceData data{ askPrice, bidPrice };

		priceDataMap.emplace(tradablePairLookup[name], std::move(data));
	}

	return priceDataMap;
}

double KrakenMarketData::get_fee() const
{
	return 0.27; // Temporary
}

const std::vector<TradablePair> KrakenMarketData::get_tradable_pairs() const
{
	std::string result = httpService.get("https://api.kraken.com/0/public/AssetPairs");
	return read_tradable_pairs(result);
}

const std::unordered_map<TradablePair, PriceData> KrakenMarketData::get_price_data(const std::vector<TradablePair>& tradablePairs) const
{
	std::string uri = build_price_data_uri(tradablePairs);
	std::string result = httpService.get(uri);
	return read_price_data(result, tradablePairs);
}

const std::unordered_map<std::string, double> KrakenTrader::get_all_balances() const
{
	return std::unordered_map<std::string, double>();
}

double KrakenTrader::get_balance(const std::string& tickerId) const
{
	return 0.0;
}

const TradeResult KrakenTrader::trade(const TradeDescription& description, double volume, double price)
{
	return TradeResult(true);
}
