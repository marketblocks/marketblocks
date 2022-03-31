#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>

#include "kraken_config.h"
#include "kraken_websocket.h"
#include "exchanges/exchange.h"
#include "exchanges/exchange_ids.h"
#include "common/utils/retry.h"
#include "networking/url.h"
#include "networking/http/http_service.h"
#include "networking/websocket/websocket_connection.h"
#include "trading/trade_description.h"
#include "trading/trading_constants.h"
#include "trading/order_book.h"

namespace mb
{
	namespace internal
	{
		struct kraken_constants
		{
		private:
			struct general_constants
			{
				static constexpr std::string_view BASEURL = "https://api.kraken.com";
				static constexpr std::string_view VERSION = "0";
				static constexpr std::string_view PUBLIC = "public";
				static constexpr std::string_view PRIVATE = "private";
			};

			struct method_constants
			{
				static constexpr std::string_view TRADABLE_PAIRS = "AssetPairs";
				static constexpr std::string_view TICKER = "Ticker";
				static constexpr std::string_view ORDER_BOOK = "Depth";
				static constexpr std::string_view BALANCE = "Balance";
				static constexpr std::string_view TRADE_VOLUME = "TradeVolume";
				static constexpr std::string_view SYSTEM_STATUS = "SystemStatus";
				static constexpr std::string_view OPEN_ORDERS = "OpenOrders";
				static constexpr std::string_view CLOSED_ORDERS = "ClosedOrders";
				static constexpr std::string_view QUERY_ORDERS = "QueryOrders";
				static constexpr std::string_view ADD_ORDER = "AddOrder";
				static constexpr std::string_view CANCEL_ORDER = "CancelOrder";
			};

			struct query_constants
			{
				static constexpr std::string_view PAIR = "pair";
				static constexpr std::string_view COUNT = "count";
				static constexpr std::string_view TYPE = "type";
				static constexpr std::string_view ORDER_TYPE = "ordertype";
				static constexpr std::string_view PRICE = "price";
				static constexpr std::string_view VOLUME = "volume";
				static constexpr std::string_view TXID = "txid";
				static constexpr std::string_view NONCE = "nonce";
			};

			struct http_constants
			{
				static constexpr std::string_view API_KEY_HEADER = "API-Key";
				static constexpr std::string_view API_SIGN_HEADER = "API-Sign";
			};

		public:
			const general_constants general;
			const method_constants methods;
			const query_constants queryKeys;
			const http_constants http;

			constexpr kraken_constants()
				: general{}, methods{}, queryKeys{}, http{}
			{}
		};

		inline bool should_retry(std::string_view errorMessage)
		{
			static unordered_string_map<bool> errorBehaviours
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

		template<typename Value, typename ResponseReader>
		result<Value> http_retry_result_converter(const http_response& response, const ResponseReader& reader)
		{
			if (response.response_code() != HttpResponseCodes::OK)
			{
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

			throw cb_exception{ responseResult.error() };
		}
	}
	
	class kraken_api : public exchange
	{
	private:
		internal::kraken_constants _constants;

		std::string _publicKey;
		std::vector<unsigned char> _decodedPrivateKey;
		int _httpRetries;
		std::unique_ptr<http_service> _httpService;

		std::string get_nonce() const;
		std::string compute_api_sign(std::string_view uriPath, std::string_view postData, std::string_view nonce) const;

		constexpr std::string build_kraken_url_path(std::string_view access, std::string_view method) const
		{
			return build_url_path(std::array<std::string_view, 3>{ _constants.general.VERSION, access, method });
		}

		constexpr std::string build_kraken_url(std::string_view access, std::string_view method, std::string_view query) const
		{
			return build_url(
				_constants.general.BASEURL,
				build_kraken_url_path(access, method),
				query);
		}

		template<typename Value, typename ResponseReader>
		Value send_request(const http_request& request, const ResponseReader& reader) const
		{
			return retry_on_fail<Value>(
				[this, &request]() { return _httpService->send(request); },
				[this, &reader](const http_response& response) { return internal::http_retry_result_converter<Value>(response, reader); },
				_httpRetries);
		}

		template<typename Value, typename ResponseReader>
		Value send_public_request(std::string_view method, const ResponseReader& reader, std::string_view query = "") const
		{
			http_request request{ http_verb::GET, build_kraken_url(_constants.general.PUBLIC, method, query) };
			return send_request<Value>(request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_private_request(std::string_view method, const ResponseReader& reader, std::string_view query = "") const
		{
			std::string nonce{ get_nonce() };

			std::string postData{ _constants.queryKeys.NONCE };
			postData.append("=" + nonce);

			if (!query.empty())
			{
				postData.append("&");
				postData.append(query);
			}

			std::string apiPath{ build_kraken_url_path(_constants.general.PRIVATE, method) };
			std::string apiSign{ compute_api_sign(apiPath, postData, nonce) };

			http_request request{ http_verb::POST, std::string{ _constants.general.BASEURL } + apiPath };
			request.add_header(_constants.http.API_KEY_HEADER, _publicKey);
			request.add_header(_constants.http.API_SIGN_HEADER, apiSign);
			request.add_header(common_http_headers::CONTENT_TYPE, common_http_headers::APPLICATION_URLENCODED);
			request.add_header(common_http_headers::ACCEPT, common_http_headers::APPLICATION_JSON);
			request.set_content(postData);

			return send_request<Value>(request, reader);
		}

	public:
		kraken_api(
			kraken_config config, 
			std::unique_ptr<http_service> httpService, 
			websocket_stream websocketStream);

		constexpr std::string_view id() const noexcept override { return exchange_ids::KRAKEN; }
		
		exchange_status get_status() const override;
		std::vector<tradable_pair> get_tradable_pairs() const override;
		pair_stats get_24h_stats(const tradable_pair& tradablePair) const override;
		order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const override;
		double get_fee(const tradable_pair& tradablePair) const override;
		unordered_string_map<double> get_balances() const override;
		std::vector<order_description> get_open_orders() const override;
		std::vector<order_description> get_closed_orders() const override;
		std::string add_order(const trade_description& description) override;
		void cancel_order(std::string_view orderId) override;
	};

	std::unique_ptr<exchange> make_kraken(kraken_config config, std::shared_ptr<websocket_client> websocketClient);
} 