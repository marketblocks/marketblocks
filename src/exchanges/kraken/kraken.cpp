#include <chrono>

#include "kraken.h"
#include "kraken_results.h"
#include "kraken_websocket.h"
#include "networking/url.h"
#include "common/utils/stringutils.h"
#include "common/utils/containerutils.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"

KrakenApi::KrakenApi(KrakenConfig config, HttpService httpService, std::shared_ptr<WebsocketClient> websocketClient)
	: Exchange{ ExchangeId::KRAKEN },
	_constants{},
	_publicKey{ config.public_key() },
	_decodedPrivateKey{ b64_decode(config.private_key()) },
	_httpService{ std::move(httpService) },
	_websocketClient{ websocketClient },
	_websocketStream{}
{}

std::string KrakenApi::build_url_path(const std::string& access, const std::string& method) const
{
	return "/" + _constants.VERSION + "/" + access + "/" + method;
}

std::string KrakenApi::build_kraken_url(const std::string& access, const std::string& method, const std::string& query) const
{
	std::string path = build_url_path(access, method);

	return build_url(_constants.BASEURL, path, query);
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

	return b64_encode(hmac_sha512(message, _decodedPrivateKey));
}

std::string KrakenApi::send_public_request(const std::string& method, const std::string& _query) const
{
	HttpRequest request{ HttpVerb::GET, build_kraken_url(_constants.PUBLIC, method, _query) };
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
	std::string apiPath = build_url_path(_constants.PRIVATE, method);
	std::string apiSign = compute_api_sign(apiPath, postData, nonce);

	HttpRequest request{ HttpVerb::POST, build_url(_constants.BASEURL, apiPath, query) };
	request.add_header("API-Key", _publicKey);
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
	std::string response = send_public_request(_constants.TRADABLE_PAIRS);

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

		std::string response = send_public_request(_constants.ORDER_BOOK, _query);
		
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
	std::string response = send_private_request(_constants.BALANCE);

	return read_balances(response);
}

TradeResult KrakenApi::trade(const TradeDescription& description)
{
	return TradeResult::SUCCESS;
}

WebsocketStream& KrakenApi::get_or_connect_websocket()
{
	if (_websocketStream.connection_status() != WsConnectionStatus::OPEN)
	{
		_websocketStream.connect(_websocketClient);
	}

	return _websocketStream;
}