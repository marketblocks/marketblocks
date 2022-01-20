#include <chrono>

#include "kraken.h"
#include "kraken_results.h"
#include "kraken_websocket.h"
#include "networking/url.h"
#include "common/utils/stringutils.h"
#include "common/utils/containerutils.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"

namespace cb
{
	kraken_api::kraken_api(kraken_config config, http_service httpService, std::shared_ptr<websocket_client> websocketClient)
		: exchange{ exchange_id::KRAKEN },
		_constants{},
		_publicKey{ config.public_key() },
		_decodedPrivateKey{ b64_decode(config.private_key()) },
		_httpService{ std::move(httpService) },
		_websocketClient{ websocketClient },
		_websocketStream{}
	{}

	std::string kraken_api::build_url_path(const std::string& access, const std::string& method) const
	{
		return "/" + _constants.VERSION + "/" + access + "/" + method;
	}

	std::string kraken_api::build_kraken_url(const std::string& access, const std::string& method, const std::string& query) const
	{
		std::string path = build_url_path(access, method);

		return build_url(_constants.BASEURL, path, query);
	}

	std::string kraken_api::get_nonce() const
	{
		return std::to_string(
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now().time_since_epoch())
			.count());
	}

	std::string kraken_api::compute_api_sign(const std::string& uriPath, const std::string& urlPostData, const std::string& nonce) const
	{
		std::vector<unsigned char> nonce_postData = sha256(nonce + urlPostData);

		std::vector<unsigned char> message{ uriPath.begin(), uriPath.end() };
		message.insert(message.end(), nonce_postData.begin(), nonce_postData.end());

		return b64_encode(hmac_sha512(message, _decodedPrivateKey));
	}

	std::string kraken_api::send_public_request(const std::string& method, const std::string& _query) const
	{
		http_request request{ http_verb::GET, build_kraken_url(_constants.PUBLIC, method, _query) };
		http_response result = _httpService.send(request);

		return result.message();
	}

	std::string kraken_api::send_public_request(const std::string& method) const
	{
		return send_public_request(method, "");
	}

	std::string kraken_api::send_private_request(const std::string& method, const std::string& query) const
	{
		std::string nonce = get_nonce();
		std::string postData = "nonce=" + nonce;
		std::string apiPath = build_url_path(_constants.PRIVATE, method);
		std::string apiSign = compute_api_sign(apiPath, postData, nonce);

		http_request request{ http_verb::POST, build_url(_constants.BASEURL, apiPath, query) };
		request.add_header("API-Key", _publicKey);
		request.add_header("API-Sign", apiSign);
		request.add_header("Content-Type", "application/x-www-form-urlencoded; charset=utf-8");
		request.set_content(postData);

		http_response result = _httpService.send(request);
		return result.message();
	}

	std::string kraken_api::send_private_request(const std::string& method) const
	{
		return send_private_request(method, "");
	}

	exchange_status kraken_api::get_status() const
	{
		std::string response = send_public_request(_constants.SYSTEM_STATUS);

		return internal::read_system_status(response);
	}

	const std::vector<tradable_pair> kraken_api::get_tradable_pairs() const
	{
		std::string response = send_public_request(_constants.TRADABLE_PAIRS);

		return internal::read_tradable_pairs(response);
	}

	const std::unordered_map<tradable_pair, order_book_state> kraken_api::get_order_book(const std::vector<tradable_pair>& tradablePairs, int depth) const
	{
		std::unordered_map<tradable_pair, order_book_state> orderBooks;
		orderBooks.reserve(depth);

		for (auto& pair : tradablePairs)
		{
			std::string _query = url_query_builder{}
				.add_parameter("pair", pair.exchange_identifier())
				.add_parameter("count", std::to_string(depth))
				.to_string();

			std::string response = send_public_request(_constants.ORDER_BOOK, _query);

			orderBooks.emplace(pair, internal::read_order_book(response, pair, depth));
		}

		return orderBooks;
	}

	const std::unordered_map<tradable_pair, double> kraken_api::get_fees(const std::vector<tradable_pair>& tradablePairs) const
	{
		return std::unordered_map<tradable_pair, double>();
	}

	const std::unordered_map<asset_symbol, double> kraken_api::get_balances() const
	{
		std::string response = send_private_request(_constants.BALANCE);

		return internal::read_balances(response);
	}

	trade_result kraken_api::trade(const trade_description& description)
	{
		return trade_result::SUCCESS;
	}

	websocket_stream& kraken_api::get_or_connect_websocket()
	{
		if (_websocketStream.connection_status() != ws_connection_status::OPEN)
		{
			_websocketStream.connect(_websocketClient);
		}

		return _websocketStream;
	}

	std::unique_ptr<exchange> make_kraken(kraken_config config, std::shared_ptr<websocket_client> websocketClient)
	{
		return std::make_unique<kraken_api>(std::move(config), http_service{}, websocketClient);
	}
}