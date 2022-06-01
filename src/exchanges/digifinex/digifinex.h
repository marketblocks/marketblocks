#pragma once

#include "digifinex_config.h"
#include "digifinex_results.h"
#include "exchanges/exchange.h"
#include "exchanges/exchange_ids.h"
#include "exchanges/exchange_common.h"
#include "networking/http/http_service.h"
#include "networking/url.h"
#include "common/utils/timeutils.h"
#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	namespace internal
	{
		struct digifinex_constants
		{
		private:
			struct general_constants
			{
				static constexpr std::string_view BASE_URL = "https://openapi.digifinex.com/v3";
			};

			struct method_constants
			{
				static constexpr std::string_view SYMBOLS = "spot/symbols";
				static constexpr std::string_view ASSETS = "spot/assets";
			};

			struct http_constants
			{
				static constexpr std::string_view ACCESS_KEY_HEADER = "ACCESS-KEY";
				static constexpr std::string_view ACCESS_TIMESTAMP_HEADER = "ACCESS-TIMESTAMP";
				static constexpr std::string_view ACCESS_SIGN_HEADER = "ACCESS-SIGN";
			};

		public:
			general_constants general;
			method_constants methods;
			http_constants http;

			constexpr digifinex_constants()
				: general{}, methods{}, http{}
			{}
		};
	}

	class digifinex_api : public exchange
	{
	private:
		internal::digifinex_constants _constants;

		std::string _apiKey;
		std::string _apiSecret;
		std::unique_ptr<http_service> _httpService;
		std::shared_ptr<websocket_stream> _websocketStream;

		std::string compute_api_sign(std::string_view query) const;

		template<typename Value, typename ResponseReader>
		Value send_public_request(std::string_view path, const ResponseReader& reader, std::string_view query = "") const
		{
			http_request request{ http_verb::GET, build_url(_constants.general.BASE_URL, path, query) };
			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_private_request(std::string_view path, const ResponseReader& reader, std::string_view query = "") const
		{
			http_request request{ http_verb::GET, build_url(_constants.general.BASE_URL, path, query) };

			std::string timestamp{ std::to_string(time_since_epoch<std::chrono::seconds>()) };

			request.add_header(_constants.http.ACCESS_KEY_HEADER, _apiKey);
			request.add_header(_constants.http.ACCESS_TIMESTAMP_HEADER, timestamp);
			request.add_header(_constants.http.ACCESS_SIGN_HEADER, compute_api_sign(query));

			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

	public:
		digifinex_api(
			digifinex_config config,
			std::unique_ptr<http_service> httpService,
			std::shared_ptr<websocket_stream> websocketStream);

		constexpr std::string_view id() const noexcept override { return exchange_ids::DIGIFINEX; }

		std::weak_ptr<websocket_stream> get_websocket_stream() override 
		{ 
			throw not_implemented_exception{ "digifinex::get_websocket_stream" };
		}

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

	std::unique_ptr<exchange> make_digifinex(digifinex_config config, std::shared_ptr<websocket_client> websocketClient);
}