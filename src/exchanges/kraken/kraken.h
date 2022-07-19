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
#include "exchanges/exchange_common.h"
#include "common/utils/retry.h"
#include "networking/url.h"
#include "networking/http/http_service.h"
#include "networking/websocket/websocket_connection.h"
#include "trading/order_request.h"
#include "trading/trading_constants.h"
#include "trading/order_book.h"

namespace mb
{
	class kraken_api : public exchange
	{
	private:
		static constexpr std::string_view _baseUrl = "https://api.kraken.com";

		std::string _publicKey;
		std::vector<unsigned char> _decodedPrivateKey;
		std::unique_ptr<http_service> _httpService;

		std::string get_nonce() const;
		std::string compute_api_sign(std::string_view uriPath, std::string_view postData, std::string_view nonce) const;
		std::string build_kraken_path(std::string access, std::string method) const;

		template<typename Value, typename ResponseReader>
		Value send_public_request(std::string method, const ResponseReader& reader, std::string_view query = "") const
		{
			std::string path{ build_kraken_path("public", std::move(method)) };
			std::string url{ build_url(_baseUrl, path, query) };

			http_request request{ http_verb::GET, std::move(url) };
			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_private_request(std::string method, const ResponseReader& reader, std::string_view query = "") const
		{
			std::string nonce{ get_nonce() };
			std::string postData{ "nonce=" + nonce};
			
			if (!query.empty())
			{
				postData.append("&");
				postData.append(query);
			}

			std::string apiPath{ build_kraken_path("private", std::move(method)) };
			std::string apiSign{ compute_api_sign(apiPath, postData, nonce) };
			std::string url{ build_url(_baseUrl, apiPath) };

			http_request request{ http_verb::POST, std::move(url) };
			request.add_header("API-Key", _publicKey);
			request.add_header("API-Sign", apiSign);
			request.add_header(common_http_headers::CONTENT_TYPE, common_http_headers::APPLICATION_URLENCODED);
			request.add_header(common_http_headers::ACCEPT, common_http_headers::APPLICATION_JSON);
			request.set_content(postData);

			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

	public:
		kraken_api(
			kraken_config config, 
			std::unique_ptr<http_service> httpService,
			std::shared_ptr<websocket_stream> websocketStream,
			bool enableTesting);

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
	std::unique_ptr<exchange> create_exchange_api<kraken_api>(bool testing);
} 