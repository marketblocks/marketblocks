#pragma once

#include "binance_config.h"
#include "binance_websocket.h"
#include "exchanges/exchange.h"
#include "exchanges/exchange_common.h"
#include "networking/http/http_service.h"
#include "networking/url.h"
#include "common/utils/timeutils.h"

namespace mb
{
	class binance_api : public exchange
	{
	public:
		std::string compute_api_sign(std::string query) const;
	private:
		std::string_view _baseUrl;
		std::string _apiKey;
		std::string _secretKey;
		std::unique_ptr<http_service> _httpService;


		template<typename Value, typename ResponseReader>
		Value send_public_request(std::string_view path, const ResponseReader& reader, std::string_view query = "") const
		{
			http_request request{ http_verb::GET, build_url(_baseUrl, path, query) };
			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_private_request(http_verb verb, std::string_view path, const ResponseReader& reader, url_query_builder query = url_query_builder{}) const
		{
			std::string timeStamp{ std::to_string(time_since_epoch<std::chrono::milliseconds>()) };
			query.add_parameter("timestamp", std::move(timeStamp));

			std::string sign{ compute_api_sign(query.to_string()) };
			query.add_parameter("signature", std::move(sign));

			http_request request{ verb, build_url(_baseUrl, path, query.to_string()) };

			request.add_header("X-MBX-APIKEY", _apiKey);

			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

	public:
		binance_api(
			binance_config config,
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
	std::unique_ptr<exchange> create_exchange_api<binance_api>(bool testing);
}