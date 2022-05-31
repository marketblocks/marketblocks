#pragma once

#include "exchanges/exchange.h"
#include "exchanges/exchange_ids.h"
#include "networking/http/http_service.h"

namespace mb
{
	class bybit_api : public exchange
	{
	private:
		std::unique_ptr<http_service> _httpService;
		std::shared_ptr<websocket_stream> _websocketStream;

	public:
		bybit_api(
			std::unique_ptr<http_service> httpService,
			std::shared_ptr<websocket_stream> websocketStream);

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

	std::unique_ptr<exchange> make_bybit(std::shared_ptr<websocket_client> websocketClient);
}