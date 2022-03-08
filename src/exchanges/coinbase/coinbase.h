#pragma once

#include "string_view"

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
				static constexpr std::string_view BASE_URL = "https://api.exchange.coinbase.com/";
			};

			struct method_constants
			{
				static constexpr std::string_view PRODUCTS = "products";
			};

		public:
			general_constants general;
			method_constants methods;

			constexpr coinbase_constants()
				: general{}, methods{}
			{}
		};
	}

	class coinbase_api final : public exchange
	{
	private:
		internal::coinbase_constants _constants;
		std::unique_ptr<http_service> _httpService;
		std::unique_ptr<websocket_stream> _websocketStream;

		template<typename Value, typename ResponseReader>
		Value send_request(const http_request& request, const ResponseReader& reader) const
		{
			http_response response{ _httpService->send(request) };

			if (response.response_code() != HttpResponseCodes::OK)
			{
				throw cb_exception{ response.message() };
			}

			return reader(response.message());
		}

		template<typename Value, typename ResponseReader>
		Value send_public_request(std::string_view method, std::string_view query, const ResponseReader& reader) const
		{
			http_request request{ http_verb::GET, build_url(_constants.general.BASE_URL, method, query) };
			return send_request<Value>(request, reader);
		}

		template<typename Value, typename ResponseReader>
		Value send_public_request(std::string_view method, const ResponseReader& reader) const
		{
			return send_public_request<Value>(method, "", reader);
		}

	public:
		coinbase_api(
			std::unique_ptr<http_service> httpService, 
			std::unique_ptr<websocket_stream> websocketStream);

		constexpr std::string_view id() const noexcept override { return exchange_ids::COINBASE; }
		websocket_stream& get_websocket_stream() noexcept override { return *_websocketStream; }

		exchange_status get_status() const override;
		std::vector<tradable_pair> get_tradable_pairs() const override;
		ticker_data get_ticker_data(const tradable_pair& tradablePair) const override;
		order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const override;
		double get_fee(const tradable_pair& tradablePair) const override;
		std::unordered_map<asset_symbol, double> get_balances() const override;
		std::vector<order_description> get_open_orders() const override;
		std::vector<order_description> get_closed_orders() const override;
		std::string add_order(const trade_description& description) override;
		void cancel_order(std::string_view orderId) override;
	};

	std::unique_ptr<exchange> make_coinbase(std::shared_ptr<websocket_client> websocketClient);
}