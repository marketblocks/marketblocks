#pragma once

#include <type_traits>

#include "exchange.h"
#include "testing/paper_trading/paper_trade_api.h"
#include "testing/back_testing/back_test_market_api.h"

namespace mb
{
	template<typename MarketApi, typename TradeApi>
	class multi_component_exchange final : public exchange
	{
	private:
		std::shared_ptr<MarketApi> _marketApi;
		std::shared_ptr<TradeApi> _tradeApi;

	public:
		multi_component_exchange(
			std::string_view id,
			std::shared_ptr<websocket_stream> websocketStream,
			std::shared_ptr<MarketApi> marketApi,
			std::shared_ptr<TradeApi> tradeApi)
			:
			exchange{ std::move(id), std::move(websocketStream) },
			_marketApi{ std::move(marketApi) },
			_tradeApi{ std::move(tradeApi) }
		{}

		std::shared_ptr<MarketApi> market_api() const noexcept
		{
			return _marketApi;
		}

		std::shared_ptr<TradeApi> trade_api() const noexcept
		{
			return _tradeApi;
		}

		exchange_status get_status() const override
		{
			return _marketApi->get_status();
		}

		std::vector<tradable_pair> get_tradable_pairs() const override
		{
			return _marketApi->get_tradable_pairs();
		}

		std::vector<ohlcv_data> get_ohlcv(const tradable_pair& tradablePair, ohlcv_interval interval, int count) const override
		{
			return _marketApi->get_ohlcv(tradablePair, interval, count);
		}

		double get_price(const tradable_pair& tradablePair) const override
		{
			return _marketApi->get_price(tradablePair);
		}

		order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const override
		{
			return _marketApi->get_order_book(tradablePair, depth);
		}

		unordered_string_map<double> get_balances() const override
		{
			return _tradeApi->get_balances();
		}

		double get_fee(const tradable_pair& tradablePair) const override
		{
			return _tradeApi->get_fee(tradablePair);
		}

		std::vector<order_description> get_open_orders() const override
		{
			return _tradeApi->get_open_orders();
		}

		std::vector<order_description> get_closed_orders() const override
		{
			return _tradeApi->get_closed_orders();
		}

		std::string add_order(const order_request& description) override
		{
			return _tradeApi->add_order(description);
		}

		void cancel_order(std::string_view orderId) override
		{
			return _tradeApi->cancel_order(orderId);
		}
	};

	using live_test_exchange = multi_component_exchange<exchange, paper_trade_api>;
	using back_test_exchange = multi_component_exchange<back_test_market_api, paper_trade_api>;
}