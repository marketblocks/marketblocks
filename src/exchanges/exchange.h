#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "exchange_status.h"
#include "websockets/websocket_stream.h"
#include "trading/tradable_pair.h"
#include "trading/trading_constants.h"
#include "trading/order_book.h"
#include "trading/order_request.h"
#include "trading/ohlcv_data.h"
#include "trading/order_description.h"

namespace mb
{
	class market_api
	{
	public:
		virtual ~market_api() = default;

		virtual exchange_status get_status() const = 0;
		virtual std::vector<tradable_pair> get_tradable_pairs() const = 0;
		virtual std::vector<ohlcv_data> get_ohlcv(const tradable_pair& tradablePair, ohlcv_interval interval, int count) const = 0;
		virtual double get_price(const tradable_pair& tradablePair) const = 0;
		virtual std::unordered_map<tradable_pair, double> get_prices(const std::vector<tradable_pair>& pairs) const;
		virtual order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const = 0;
		virtual std::unordered_map<tradable_pair, order_book_state> get_order_books(const std::vector<tradable_pair>& pairs, int depth) const;
	};

	class trade_api
	{
	public:
		virtual ~trade_api() = default;

		virtual std::unordered_map<std::string, double> get_balances() const = 0;
		virtual double get_fee(const tradable_pair& tradablePair) const = 0;
		virtual std::vector<order_description> get_open_orders() const = 0;
		virtual std::vector<order_description> get_closed_orders() const = 0;
		virtual std::string add_order(const order_request& description) = 0;
		virtual void cancel_order(std::string_view orderId) = 0;
	};

	class exchange : public market_api, public trade_api
	{
	private:
		std::string_view _id;
		std::shared_ptr<websocket_stream> _websocketStream;
		bool _websocketConnected;

	public:
		exchange(std::string_view id, std::shared_ptr<websocket_stream> websocketStream);

		virtual ~exchange() = default;

		constexpr std::string_view id() const noexcept { return _id; }
		std::shared_ptr<websocket_stream> get_websocket_stream();
	};	

	template<typename T>
	std::unique_ptr<exchange> create_exchange_api(bool testing = false)
	{
		static_assert(sizeof(T) == 0, "No specialization of create_exchange_api found");
	}
}