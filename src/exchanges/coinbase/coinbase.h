#pragma once

#include "string_view"

#include "coinbase_config.h"
#include "coinbase_websocket.h"
#include "exchanges/exchange.h"
#include "exchanges/exchange_ids.h"
#include "networking/http/http_service.h"
#include "networking/url.h"
#include "common/utils/retry.h"

namespace cb
{
	namespace internal
	{
		struct coinbase_constants
		{
		private:
			struct general_constants
			{
			private:
				static constexpr std::string_view LIVE_BASE_URL = "https://api.exchange.coinbase.com";
				static constexpr std::string_view SANDBOX_BASE_URL = "https://api-public.sandbox.exchange.coinbase.com";

				constexpr std::string_view select_base_url(bool enableTesting)
				{
					if (enableTesting)
					{
						return SANDBOX_BASE_URL;
					}
					return LIVE_BASE_URL;
				}

			public:
				const std::string_view BASE_URL;

				constexpr general_constants(bool enableTesting)
					: BASE_URL{ select_base_url(enableTesting) }
				{}
			};

			struct method_constants
			{
				static constexpr std::string_view PRODUCTS = "products";
				static constexpr std::string_view STATS = "stats";
				static constexpr std::string_view BOOK = "book";
				static constexpr std::string_view FEES = "fees";
				static constexpr std::string_view COINBASE_ACCOUNTS = "coinbase-accounts"; 
				static constexpr std::string_view ORDERS = "orders"; 
			};

			struct query_constants
			{
				static constexpr std::string_view LEVEL = "level";
				static constexpr std::string_view STATUS = "status";
			};

			struct http_constants
			{
				static constexpr std::string_view ACCESS_KEY_HEADER = "CB-ACCESS-KEY";
				static constexpr std::string_view ACCESS_SIGN_HEADER = "CB-ACCESS-SIGN";
				static constexpr std::string_view ACCESS_TIMESTAMP_HEADER = "CB-ACCESS-TIMESTAMP";
				static constexpr std::string_view ACCESS_PASSPHRASE_HEADER = "CB-ACCESS-PASSPHRASE";
			};

		public:
			general_constants general;
			method_constants methods;
			query_constants queries;
			http_constants http;

			constexpr coinbase_constants(bool enableTesting)
				: general{ enableTesting }, methods{}, queries{}, http{}
			{}
		};

		constexpr std::string to_exchange_id(const cb::tradable_pair& pair)
		{
			return pair.asset() + "-" + pair.price_unit();
		}
	}

	class coinbase_api final : public exchange
	{
	private:
		internal::coinbase_constants _constants;

		std::string _userAgentId;
		std::string _apiKey;
		std::vector<unsigned char> _decodedApiSecret;
		std::string _apiPassphrase;
		std::unique_ptr<http_service> _httpService;

		std::string get_timestamp() const;
		std::string compute_access_sign(std::string_view timestamp, http_verb httpVerb, std::string_view path, std::string_view query, std::string_view body) const;

		template<typename Value, typename ResponseReader>
		Value send_request(http_request& request, const ResponseReader& reader) const
		{
			request.add_header(common_http_headers::USER_AGENT, _userAgentId);
			request.add_header(common_http_headers::ACCEPT, common_http_headers::APPLICATION_JSON);
			
			http_response response{ _httpService->send(request) };

			if (response.response_code() != HttpResponseCodes::OK)
			{
				throw cb_exception{ response.message() };
			}

			result<Value> result{ reader(response.message()) };

			if (result.is_success())
			{
				return result.value();
			}

			throw cb_exception{ result.error() };
		}

		template<typename Value, typename ResponseReader>
		Value send_public_request(std::string_view path, const ResponseReader& reader, std::string_view query = "") const
		{
			http_request request{ http_verb::GET, build_url(_constants.general.BASE_URL, path, query) };
			return send_request<Value>(request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_private_request(http_verb httpVerb, std::string_view path, const ResponseReader& reader, std::string_view query = "", std::string_view content = "") const
		{
			http_request request{ httpVerb, build_url(_constants.general.BASE_URL, path, query) };

			if (!content.empty())
			{
				request.set_content(content);
				request.add_header(common_http_headers::CONTENT_TYPE, common_http_headers::APPLICATION_JSON);
			}

			std::string timestamp{ get_timestamp() };

			request.add_header(_constants.http.ACCESS_KEY_HEADER, _apiKey);
			request.add_header(_constants.http.ACCESS_SIGN_HEADER, compute_access_sign(timestamp, httpVerb, path, query, content));
			request.add_header(_constants.http.ACCESS_TIMESTAMP_HEADER, timestamp);
			request.add_header(_constants.http.ACCESS_PASSPHRASE_HEADER, _apiPassphrase);

			return send_request<Value>(request, reader);
		}

	public:
		coinbase_api(
			coinbase_config config,
			std::unique_ptr<http_service> httpService, 
			websocket_stream websocketStream,
			bool enableTesting = false);

		constexpr std::string_view id() const noexcept override { return exchange_ids::COINBASE; }

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

	std::unique_ptr<exchange> make_coinbase(coinbase_config config, std::shared_ptr<websocket_client> websocketClient, bool enableTesting = false);
}