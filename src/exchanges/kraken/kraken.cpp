#include <chrono>

#include "kraken.h"
#include "kraken_results.h"
#include "kraken_websocket.h"
#include "networking/http/http_constants.h"
#include "common/types/result.h"
#include "common/utils/stringutils.h"
#include "common/utils/containerutils.h"
#include "common/utils/timeutils.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"

namespace cb
{
	kraken_api::kraken_api(kraken_config config, http_service httpService, kraken_websocket_stream websocketStream)
		: exchange{ exchange_id::KRAKEN },
		_constants{},
		_publicKey{ config.public_key() },
		_decodedPrivateKey{ b64_decode(config.private_key()) },
		_httpRetries{ config.http_retries() },
		_httpService{ std::move(httpService) },
		_websocketStream{ std::move(websocketStream) }
	{}

	std::string kraken_api::build_url_path(const std::string& access, const std::string& method) const
	{
		return "/" + _constants.VERSION + "/" + access + "/" + method;
	}

	std::string kraken_api::build_kraken_url(const std::string& access, const std::string& method, const std::string& query) const
	{
		std::string path{ build_url_path(access, method) };

		return build_url(_constants.BASEURL, path, query);
	}

	std::string kraken_api::get_nonce() const
	{
		return std::to_string(milliseconds_since_epoch());
	}

	std::string kraken_api::compute_api_sign(const std::string& uriPath, const std::string& urlPostData, const std::string& nonce) const
	{
		std::vector<unsigned char> nonce_postData = sha256(nonce + urlPostData);

		std::vector<unsigned char> message{ uriPath.begin(), uriPath.end() };
		message.insert(message.end(), nonce_postData.begin(), nonce_postData.end());

		return b64_encode(hmac_sha512(message, _decodedPrivateKey));
	}

	exchange_status kraken_api::get_status() const
	{
		return send_public_request<exchange_status>(_constants.SYSTEM_STATUS, internal::read_system_status);
	}

	const std::vector<tradable_pair> kraken_api::get_tradable_pairs() const
	{
		return send_public_request<std::vector<tradable_pair>>(_constants.TRADABLE_PAIRS, internal::read_tradable_pairs);
	}

	const order_book_state kraken_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		std::string _query = url_query_builder{}
			.add_parameter("pair", tradablePair.exchange_identifier())
			.add_parameter("count", std::to_string(depth))
			.to_string();

		return send_public_request<order_book_state>(_constants.ORDER_BOOK, _query, internal::read_order_book);
	}

	const std::unordered_map<tradable_pair, double> kraken_api::get_fees(const std::vector<tradable_pair>& tradablePairs) const
	{
		return std::unordered_map<tradable_pair, double>();
	}

	const std::unordered_map<asset_symbol, double> kraken_api::get_balances() const
	{
		return send_private_request<std::unordered_map<asset_symbol, double>>(_constants.BALANCE, internal::read_balances);
	}

	trade_result kraken_api::trade(const trade_description& description)
	{
		return trade_result::SUCCESS;
	}

	websocket_stream& kraken_api::get_websocket_stream()
	{
		return _websocketStream;
	}

	std::unique_ptr<exchange> make_kraken(kraken_config config, std::shared_ptr<websocket_client> websocketClient)
	{
		return std::make_unique<kraken_api>(std::move(config), http_service{}, kraken_websocket_stream{ websocketClient });
	}
}