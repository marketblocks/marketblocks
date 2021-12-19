#include <chrono>

#include "kraken.h"
#include "kraken_results.h"
#include "networking/json_wrapper.h"
#include "networking/url.h"
#include "utils/stringutils.h"
#include "utils/containerutils.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"

namespace 
{
	const std::string publicApiKey = "";
	const std::string privateApiKey = "";

	namespace Urls
	{
		static const std::string BASE = "https://api.kraken.com";
		static const std::string VERSION = "0";
		static const std::string PUBLIC = "public";
		static const std::string PRIVATE = "private";

		static const std::string TRADABLE_PAIRS = "AssetPairs";
		static const std::string ORDER_BOOK = "Depth";
		static const std::string BALANCE = "Balance";
	}

	std::string build_path(const std::string& access, const std::string& method)
	{
		return "/" + Urls::VERSION + "/" + access + "/" + method;
	}
	
	void append_query(std::string& url, const std::string& _query)
	{
		if (!_query.empty())
		{
			url += "?" + _query;
		}
	}

	std::string build_url(const std::string& path, const std::string& _query)
	{
		std::string url = Urls::BASE + path;
		
		append_query(url, _query);

		return url;
	}

	std::string build_url(const std::string& access, const std::string& method, const std::string& _query)
	{
		std::string url = Urls::BASE + build_path(access, method);

		append_query(url, _query);

		return url;
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

std::string KrakenApi::send_public_request(const std::string& method, const std::string& _query) const
{
	HttpRequest request{ HttpVerb::GET, build_url(Urls::PUBLIC, method, _query) };
	HttpResponse result = _httpService.send(request);

	return result.message();
}

std::string KrakenApi::send_public_request(const std::string& method) const
{
	return send_public_request(method, "");
}

std::string KrakenApi::send_private_request(const std::string& method, const std::string& query) const
{
	std::string nonce = get_nonce();
	std::string postData = "nonce=" + nonce;
	std::string apiPath = build_path(Urls::PRIVATE, method);
	std::string apiSign = compute_api_sign(apiPath, postData, nonce);

	HttpRequest request{ HttpVerb::POST, build_url(apiPath, query) };
	request.add_header("API-Key", publicApiKey);
	request.add_header("API-Sign", apiSign);
	request.add_header("Content-Type", "application/x-www-form-urlencoded; charset=utf-8");
	request.set_content(postData);
	
	HttpResponse result = _httpService.send(request);
	return result.message();
}

std::string KrakenApi::send_private_request(const std::string& method) const
{
	return send_private_request(method, "");
}

const std::vector<TradablePair> KrakenApi::get_tradable_pairs() const
{
	std::string response = send_public_request(Urls::TRADABLE_PAIRS);

	return read_tradable_pairs(response);
}

const std::unordered_map<TradablePair, OrderBookState> KrakenApi::get_order_book(const std::vector<TradablePair>& tradablePairs, int depth) const
{
	std::unordered_map<TradablePair, OrderBookState> orderBooks;
	orderBooks.reserve(depth);

	for (auto& pair : tradablePairs)
	{
		std::string _query = UrlQueryBuilder{}
			.add_parameter("pair", pair.exchange_identifier())
			.add_parameter("count", std::to_string(depth))
			.to_string();

		std::string response = send_public_request(Urls::ORDER_BOOK, _query);
		
		orderBooks.emplace(pair, read_order_book(response, pair, depth));
	}

	return orderBooks;
}

const std::unordered_map<TradablePair, double> KrakenApi::get_fees(const std::vector<TradablePair>& tradablePairs) const
{
	return std::unordered_map<TradablePair, double>();
}

const std::unordered_map<AssetSymbol, double> KrakenApi::get_balances() const
{
	std::string response = send_private_request(Urls::BALANCE);

	return read_balances(response);
}

TradeResult KrakenApi::trade(const TradeDescription& description)
{
	return TradeResult::SUCCESS;
}