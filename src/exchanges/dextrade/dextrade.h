#pragma once

#include "dextrade_config.h"
#include "dextrade_results.h"
#include "exchanges/exchange.h"
#include "exchanges/exchange_ids.h"
#include "exchanges/exchange_common.h"
#include "networking/http/http_service.h"
#include "networking/url.h"
#include "common/utils/timeutils.h"
#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	class dextrade_api : public exchange
	{
	private:
		static constexpr std::string_view _baseUrl = "https://api.dex-trade.com/v1";

		std::string _publicKey;
		std::string _privateKey;
		std::unique_ptr<http_service> _httpService;

		std::string compute_api_sign(const json_document& queryValues) const;

		template<typename Value, typename ResponseReader>
		Value send_public_request(std::string_view path, const ResponseReader& reader, std::string_view query = "") const
		{
			std::string urlPath{ build_url_path(std::array<std::string_view, 2>{ "public", path }) };
			std::string url{ build_url(_baseUrl, urlPath, query) };

			http_request request{ http_verb::GET, std::move(url) };
			
			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_private_request(std::string_view path, const ResponseReader& reader, json_writer& queryJson) const
		{
			std::string urlPath{ build_url_path(std::array<std::string_view, 2>{"private", path}) };
			std::string url{ build_url(_baseUrl, urlPath, "")};
			http_request request{ http_verb::POST, std::move(url) };

			std::string timeStamp{ std::to_string(time_since_epoch<std::chrono::milliseconds>()) };
			queryJson.add("request_id", timeStamp);

			request.set_content(queryJson.to_string());

			std::string apiSign{ compute_api_sign(queryJson.to_json()) };
			request.add_header("content-type", "application/json");
			request.add_header("login-token", _publicKey);
			request.add_header("X-Auth-Sign", apiSign);

			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_private_request(std::string_view path, const ResponseReader& reader) const
		{
			json_writer json;
			return send_private_request<Value>(path, reader, json);
		}

	public:
		dextrade_api(
			dextrade_config config,
			std::unique_ptr<http_service> httpService,
			std::shared_ptr<websocket_stream> websocketStream);

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
	std::unique_ptr<exchange> create_exchange_api<dextrade_api>(bool testing);
}