#pragma once

#include "digifinex_config.h"
#include "digifinex_results.h"
#include "digifinex_websocket.h"
#include "exchanges/exchange.h"
#include "exchanges/exchange_ids.h"
#include "exchanges/exchange_common.h"
#include "networking/http/http_service.h"
#include "networking/url.h"
#include "common/utils/timeutils.h"
#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	class digifinex_api : public exchange
	{
	private:
		static constexpr const char _pairSeparator = '_';
		static constexpr std::string_view _baseUrl = "https://openapi.digifinex.com/v3";

		std::string _apiKey;
		std::string _apiSecret;
		std::unique_ptr<http_service> _httpService;

		std::string compute_api_sign(std::string_view query) const;

		template<typename Value, typename ResponseReader>
		Value send_public_request(std::string_view path, const ResponseReader& reader, std::string_view query = "") const
		{
			http_request request{ http_verb::GET, build_url(_baseUrl, path, query) };
			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_private_request(http_verb verb, std::string_view path, const ResponseReader& reader, std::string_view query = "") const
		{
			http_request request{ verb, build_url(_baseUrl, path, query) };

			std::string timestamp{ std::to_string(time_since_epoch<std::chrono::seconds>()) };

			request.add_header("ACCESS-KEY", _apiKey);
			request.add_header("ACCESS-TIMESTAMP", timestamp);
			request.add_header("ACCESS-SIGN", compute_api_sign(query));

			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

	public:
		digifinex_api(
			digifinex_config config,
			std::unique_ptr<http_service> httpService,
			std::shared_ptr<websocket_stream> websocketStream,
			bool enableTesting = false);

		exchange_status get_status() const override;
		std::vector<tradable_pair> get_tradable_pairs() const override;
		std::vector<ohlcv_data> get_ohlcv(const tradable_pair& tradablePair, ohlcv_interval interval, int count) const override;
		double get_price(const tradable_pair& tradablePair) const override;
		order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const override;
		double get_fee(const tradable_pair& tradablePair) const override;
		unordered_string_map<double> get_balances() const override;
		std::vector<order_description> get_open_orders() const override;
		std::vector<order_description> get_closed_orders() const override;
		std::string add_order(const order_request& description) override;
		void cancel_order(std::string_view orderId) override;
	};

	template<>
	std::unique_ptr<exchange> create_exchange_api<digifinex_api>(bool testing);
}