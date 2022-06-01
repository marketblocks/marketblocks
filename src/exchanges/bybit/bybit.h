#pragma once

#include "bybit_config.h"
#include "bybit_results.h"
#include "exchanges/exchange.h"
#include "exchanges/exchange_ids.h"
#include "exchanges/exchange_common.h"
#include "networking/http/http_service.h"
#include "networking/url.h"
#include "common/utils/timeutils.h"

namespace mb
{
	namespace internal
	{
		struct bybit_constants
		{
		private:
			struct general_constants
			{
			private:
				static constexpr std::string_view LIVE_BASE_URL = "https://api-testnet.bybit.com";
				static constexpr std::string_view TEST_BASE_URL = "https://api.bybit.com";

				constexpr std::string_view select_base_url(bool enableTesting)
				{
					if (enableTesting)
					{
						return TEST_BASE_URL;
					}
					return LIVE_BASE_URL;
				}

			public:
				const std::string_view BASE_URL;
				static constexpr std::string_view VERSION = "v2";
				static constexpr std::string_view PUBLIC = "public";
				static constexpr std::string_view PRIVATE = "private";

				constexpr general_constants(bool enableTesting)
					: BASE_URL{ select_base_url(enableTesting) }
				{}
			};

			struct method_constants
			{
				static constexpr std::string_view SYMBOLS = "symbols";
				static constexpr std::string_view BALANCE = "wallet/balance";
			};

			struct query_constants
			{
				static constexpr std::string_view API_KEY = "api_key";
				static constexpr std::string_view TIMESTAMP = "timestamp";
				static constexpr std::string_view SIGN = "sign";
			};

		public:
			general_constants general;
			method_constants methods;
			query_constants queries;

			constexpr bybit_constants(bool enableTesting)
				: general{ enableTesting }, methods{}, queries{}
			{}
		};
	}

	class bybit_api : public exchange
	{
	private:
		internal::bybit_constants _constants;

		std::string _apiKey;
		std::string _apiSecret;
		std::unique_ptr<http_service> _httpService;
		std::shared_ptr<websocket_stream> _websocketStream;

		std::string get_time_stamp() const;
		std::string compute_api_sign(std::string_view query) const;

		constexpr std::string build_bybit_url_path(std::string_view access, std::string_view method) const
		{
			return build_url_path(std::array<std::string_view, 3>{ _constants.general.VERSION, access, method });
		}

		template<typename Value, typename ResponseReader>
		Value send_public_request(std::string_view method, const ResponseReader& reader, std::string_view query = "") const
		{
			std::string path{ build_bybit_url_path(_constants.general.PUBLIC, method) };
			http_request request{ http_verb::GET, build_url(_constants.general.BASE_URL, path, query) };
			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_private_request(std::string_view method, const ResponseReader& reader, std::map<std::string_view, std::string> queryParams = {}) const
		{
			std::string path{ build_bybit_url_path(_constants.general.PRIVATE, method) };

			queryParams.emplace(_constants.queries.API_KEY, _apiKey);
			queryParams.emplace(_constants.queries.TIMESTAMP, get_time_stamp());

			url_query_builder queryBuilder{};
			for (auto& [key, value] : queryParams)
			{
				queryBuilder.add_parameter(key, value);
			}

			queryBuilder.add_parameter(_constants.queries.SIGN, compute_api_sign(queryBuilder.to_string()));
			
			std::string query{ queryBuilder.to_string() };

			http_request request{ http_verb::GET, build_url(_constants.general.BASE_URL, path, query) };
			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

	public:
		bybit_api(
			bybit_config config,
			std::unique_ptr<http_service> httpService,
			std::shared_ptr<websocket_stream> websocketStream,
			bool enableTesting = false);

		constexpr std::string_view id() const noexcept override { return exchange_ids::BYBIT; }

		std::weak_ptr<websocket_stream> get_websocket_stream() override { return _websocketStream; }

		exchange_status get_status() const override;
		std::vector<tradable_pair> get_tradable_pairs() const override;
		ohlcv_data get_24h_stats(const tradable_pair& tradablePair) const override;
		double get_price(const tradable_pair& tradablePair) const override;
		order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const override;
		double get_fee(const tradable_pair& tradablePair) const override;
		unordered_string_map<double> get_balances() const override;
		std::vector<order_description> get_open_orders() const override;
		std::vector<order_description> get_closed_orders() const override;
		std::string add_order(const trade_description& description) override;
		void cancel_order(std::string_view orderId) override;
	};

	std::unique_ptr<exchange> make_bybit(bybit_config config, std::shared_ptr<websocket_client> websocketClient, bool enableTesting = false);
}