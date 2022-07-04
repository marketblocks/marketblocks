#pragma once

#include "coinbase_config.h"
#include "coinbase_websocket.h"
#include "exchanges/exchange.h"
#include "exchanges/exchange_ids.h"
#include "exchanges/exchange_common.h"
#include "networking/http/http_service.h"
#include "networking/url.h"

namespace mb
{
	class coinbase_api final : public exchange
	{
	private:
		static constexpr const char _pairSeparator = '-';
		std::string_view _baseUrl;

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
			
			return internal::send_http_request<Value>(*_httpService, request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_public_request(std::string_view path, const ResponseReader& reader, std::string_view query = "") const
		{
			http_request request{ http_verb::GET, build_url(_baseUrl, path, query) };
			return send_request<Value>(request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_private_request(http_verb httpVerb, std::string path, const ResponseReader& reader, std::string_view query = "", std::string_view content = "") const
		{
			http_request request{ httpVerb, build_url(_baseUrl, path, query) };

			if (!content.empty())
			{
				request.set_content(content);
				request.add_header(common_http_headers::CONTENT_TYPE, common_http_headers::APPLICATION_JSON);
			}

			std::string timestamp{ get_timestamp() };

			request.add_header("CB-ACCESS-KEY", _apiKey);
			request.add_header("CB-ACCESS-SIGN", compute_access_sign(timestamp, httpVerb, path, query, content));
			request.add_header("CB-ACCESS-TIMESTAMP", timestamp);
			request.add_header("CB-ACCESS-PASSPHRASE", _apiPassphrase);

			return send_request<Value>(request, reader);
		}

	public:
		coinbase_api(
			coinbase_config config,
			std::unique_ptr<http_service> httpService, 
			std::shared_ptr<websocket_stream> websocketStream,
			bool enableTesting = false);

		exchange_status get_status() const override;
		std::vector<tradable_pair> get_tradable_pairs() const override;

		std::vector<ohlcv_data> get_ohlcv(const tradable_pair& tradablePair, ohlcv_interval interval, int count) const override
		{
			throw not_implemented_exception{ "coinbase::get_ohlcv" };
		}

		double get_price(const tradable_pair& tradablePair) const override;
		order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const override;
		double get_fee(const tradable_pair& tradablePair) const override;
		unordered_string_map<double> get_balances() const override;
		std::vector<order_description> get_open_orders() const override;
		std::vector<order_description> get_closed_orders() const override;
		std::string add_order(const trade_description& description) override;
		void cancel_order(std::string_view orderId) override;
	};

	template<>
	std::unique_ptr<exchange> create_exchange_api<coinbase_api>(bool testing);
}