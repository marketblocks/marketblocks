#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "exchange_status.h"
#include "websockets/websocket_stream.h"
#include "common/types/unordered_string_map.h"
#include "trading/tradable_pair.h"
#include "trading/trading_constants.h"
#include "trading/order_book.h"
#include "trading/trade_description.h"
#include "trading/ohlcv_data.h"
#include "trading/order_description.h"
#include "testing/paper_trading/paper_trade_api.h"
#include "testing/back_testing/backtest_market_api.h"

namespace mb
{
	class exchange
	{
	public:
		virtual ~exchange() = default;

		constexpr virtual std::string_view id() const noexcept = 0;
		
		virtual std::shared_ptr<websocket_stream> get_websocket_stream() = 0;
		virtual exchange_status get_status() const = 0;
		virtual std::vector<tradable_pair> get_tradable_pairs() const = 0;
		virtual ohlcv_data get_24h_stats(const tradable_pair& tradablePair) const = 0;
		virtual double get_price(const tradable_pair& tradablePair) const = 0;
		virtual order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const = 0;
		virtual unordered_string_map<double> get_balances() const = 0;
		virtual double get_fee(const tradable_pair& tradablePair) const = 0;
		virtual std::vector<order_description> get_open_orders() const = 0;
		virtual std::vector<order_description> get_closed_orders() const = 0;
		virtual std::string add_order(const trade_description& description) = 0;
		virtual void cancel_order(std::string_view orderId) = 0;
	};

	template<typename MarketApi, typename TradeApi>
	class multi_component_exchange final : public exchange
	{
	private:
		std::shared_ptr<MarketApi> _marketApi;
		std::shared_ptr<TradeApi> _tradeApi;

	public:
		multi_component_exchange(std::shared_ptr<MarketApi> marketApi, std::shared_ptr<TradeApi> tradeApi)
			: 
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

		constexpr std::string_view id() const noexcept
		{
			return _marketApi->id();
		}

		std::shared_ptr<websocket_stream> get_websocket_stream() override
		{
			return _marketApi->get_websocket_stream();
		}

		exchange_status get_status() const override
		{
			return _marketApi->get_status();
		}

		std::vector<tradable_pair> get_tradable_pairs() const override
		{
			return _marketApi->get_tradable_pairs();
		}

		ohlcv_data get_24h_stats(const tradable_pair& tradablePair) const override
		{
			return _marketApi->get_24h_stats(tradablePair);
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

		std::string add_order(const trade_description& description) override
		{
			return _tradeApi->add_order(description);
		}

		void cancel_order(std::string_view orderId) override
		{
			return _tradeApi->cancel_order(orderId);
		}
	};

	typedef multi_component_exchange<exchange, paper_trade_api> live_test_exchange;
	typedef multi_component_exchange<backtest_market_api, paper_trade_api> back_test_exchange;
}