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
	class bybit_api : public exchange
	{
	private:
		std::string_view _baseUrl;

		std::string _apiKey;
		std::string _apiSecret;
		std::unique_ptr<http_service> _httpService;
		std::shared_ptr<websocket_stream> _websocketStream;

		std::string get_time_stamp() const;
		std::string compute_api_sign(std::string_view query) const;

		template<typename Value, typename ResponseReader>
		Value send_public_request(std::string_view path, const ResponseReader& reader, std::string_view query = "") const
		{
			http_request request{ http_verb::GET, build_url(_baseUrl, path, query) };
			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_private_request(http_verb verb, std::string_view path, const ResponseReader& reader, std::map<std::string, std::string> queryParams = {}) const
		{
			queryParams.emplace("api_key", _apiKey);
			queryParams.emplace("timestamp", get_time_stamp());

			url_query_builder queryBuilder{};
			for (auto& [key, value] : queryParams)
			{
				queryBuilder.add_parameter(key, value);
			}

			queryBuilder.add_parameter("sign", compute_api_sign(queryBuilder.to_string()));

			std::string query;
			std::string content;

			if (verb == http_verb::POST)
			{
				content = queryBuilder.to_string();
			}
			else
			{
				query = queryBuilder.to_string();
			}

			http_request request{ verb, build_url(_baseUrl, path, query) };
			request.set_content(content);

			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

	public:
		bybit_api(
			bybit_config config,
			std::unique_ptr<http_service> httpService,
			bool enableTesting = false);

		constexpr std::string_view id() const noexcept override { return exchange_ids::BYBIT; }

		std::shared_ptr<websocket_stream> get_websocket_stream() override;
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

	std::unique_ptr<exchange> make_bybit(bybit_config config, bool enableTesting = false);
}