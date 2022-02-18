#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>

#include "kraken_config.h"
#include "kraken_websocket.h"
#include "exchanges/exchange.h"
#include "common/utils/retry.h"
#include "networking/url.h"
#include "networking/http/http_service.h"
#include "networking/websocket/websocket_connection.h"
#include "trading/trade_description.h"
#include "trading/trading_constants.h"
#include "trading/order_book.h"

namespace cb
{
	namespace internal
	{
		struct kraken_constants
		{
			inline static const std::string BASEURL = "https://api.kraken.com";
			inline static const std::string VERSION = "0";
			inline static const std::string PUBLIC = "public";
			inline static const std::string PRIVATE = "private";

			inline static const std::string TRADABLE_PAIRS = "AssetPairs";
			inline static const std::string ORDER_BOOK = "Depth";
			inline static const std::string BALANCE = "Balance";
			inline static const std::string SYSTEM_STATUS = "SystemStatus";
		};

		inline bool should_retry(std::string errorMessage)
		{
			static std::unordered_map<std::string, bool> errorBehaviours
			{
				{ "EGeneral:Permission denied", false },
				{ "EAPI:Invalid key", false },
				{ "EQuery:Unknown asset pair", false },
				{ "EGeneral:Invalid arguments", false },
				{ "EAPI:Invalid signature", false },
				{ "EAPI:Invalid nonce", false },
				{ "ESession:Invalid session", false },
				{ "EAPI:Rate limit exceeded", true },
				{ "EOrder:Rate limit exceeded", true },
				{ "EGeneral:Temporary lockout", true },
				{ "EOrder:Cannot open position", true },
				{ "EOrder:Cannot open opposing position", false },
				{ "EOrder:Margin allowance exceeded", false },
				{ "EOrder:Insufficient margin", false },
				{ "EOrder:Insufficient funds (insufficient user funds)", false },
				{ "EOrder:Order minimum not met (volume too low)", false },
				{ "EOrder:Orders limit exceeded", false },
				{ "EOrder:Positions limit exceeded", false },
				{ "EService:Unavailable", true },
				{ "EService:Busy", true },
				{ "EGeneral:Internal error", true },
				{ "ETrade:Locked", false },
				{ "EAPI:Feature disabled", false },
			};

			auto behaviourIterator = errorBehaviours.find(errorMessage);
			if (behaviourIterator == errorBehaviours.end())
			{
				return false;
			}

			return behaviourIterator->second;
		}

		template<typename Value>
		using PostDataVerifier = std::function<result<Value>()>;

		template<typename Value, typename ResponseReader>
		result<Value> http_retry_result_converter(const http_response& response, const ResponseReader& reader, const std::optional<PostDataVerifier<Value>>& postDataVerifier)
		{
			if (response.response_code() != HttpResponseCodes::OK)
			{
				if (postDataVerifier.has_value())
				{
					result<Value> postVerifiedResult{ postDataVerifier.value()() };
					if (postVerifiedResult.is_success())
					{
						logger::instance().warning("Kraken REST API call resulted in non-200 status code but data was verified as received by the server");
						return postVerifiedResult;
					}
				}

				return result<Value>::fail(response.message());
			}

			result<Value> responseResult{ reader(response.message()) };

			if (responseResult.is_success())
			{
				return responseResult;
			}

			if (internal::should_retry(responseResult.error()))
			{
				return result<Value>::fail(responseResult.error());
			}

			throw cb_exception{ std::format("Kraken API error occurred that could not be recovered from: {}", responseResult.error()) };
		}
	}
	
	class kraken_api final : public exchange
	{
	private:
		internal::kraken_constants _constants;
		
		std::string _publicKey;
		std::vector<unsigned char> _decodedPrivateKey;
		int _httpRetries;
		http_service _httpService;
		kraken_websocket_stream _websocketStream;

		std::string build_url_path(const std::string& access, const std::string& method) const;
		std::string build_kraken_url(const std::string& access, const std::string& method, const std::string& query) const;
		std::string get_nonce() const;
		std::string compute_api_sign(const std::string& uriPath, const std::string& postData, const std::string& nonce) const;

		template<typename Value, typename ResponseReader>
		Value send_request(const http_request& request, const ResponseReader& reader, const std::optional<internal::PostDataVerifier<Value>>& postDataVerifier = std::nullopt) const
		{
			return retry_on_fail<Value>(
				[this, &request]() { return _httpService.send(request); },
				[this, &reader, &postDataVerifier](const cb::http_response& response) { return internal::http_retry_result_converter<Value>(response, reader, postDataVerifier); },
				_httpRetries);
		}

		template<typename Value, typename ResponseReader>
		Value send_public_request(const std::string& method, const std::string& query, const ResponseReader& reader) const
		{
			http_request request{ http_verb::GET, build_kraken_url(_constants.PUBLIC, method, query) };
			return send_request<Value>(request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_public_request(const std::string& method, const ResponseReader& reader) const
		{
			return send_public_request<Value>(method, "", reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_private_request(const std::string& method, const std::string& query, const ResponseReader& reader, const std::optional<internal::PostDataVerifier<Value>>& postDataVerifier = std::nullopt) const
		{
			std::string nonce{ get_nonce() };
			std::string postData{ "nonce=" + nonce };
			std::string apiPath{ build_url_path(_constants.PRIVATE, method) };
			std::string apiSign{ compute_api_sign(apiPath, postData, nonce) };

			http_request request{ http_verb::POST, build_url(_constants.BASEURL, apiPath, query) };
			request.add_header("API-Key", _publicKey);
			request.add_header("API-Sign", apiSign);
			request.add_header("Content-Type", "application/x-www-form-urlencoded; charset=utf-8");
			request.set_content(postData);

			return send_request<Value>(request, reader, postDataVerifier);
		}

		template<typename Value, typename ResponseReader>
		Value send_private_request(const std::string& method, const ResponseReader& reader, const std::optional<internal::PostDataVerifier<Value>>& postDataVerifier = std::nullopt) const
		{
			return send_private_request<Value>(method, "", reader, postDataVerifier);
		}

	public:
		kraken_api(kraken_config config, http_service httpService, kraken_websocket_stream websocketStream);

		exchange_status get_status() const override;
		const std::vector<tradable_pair> get_tradable_pairs() const override;
		const order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const override;
		const std::unordered_map<tradable_pair, double> get_fees(const std::vector<tradable_pair>& tradablePairs) const override;
		const std::unordered_map<asset_symbol, double> get_balances() const override;
		trade_result trade(const trade_description& description) override;

		websocket_stream& get_websocket_stream() override;
	};

	std::unique_ptr<exchange> make_kraken(kraken_config config, std::shared_ptr<websocket_client> websocketClient);
}