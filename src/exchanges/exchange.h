#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "exchange_id.h"
#include "exchange_status.h"
#include "websockets/websocket_stream.h"

#include "trading/tradable_pair.h"
#include "trading/asset_symbol.h"
#include "trading/trading_constants.h"
#include "trading/order_book.h"
#include "trading/trade_description.h"
#include "paper_trading/paper_trader.h"

namespace cb
{
	class exchange
	{
	private:
		exchange_id _id;

	public:
		exchange(exchange_id id)
			: _id{ id }
		{}

		virtual ~exchange() = default;

		exchange_id id() const { return _id; }

		virtual exchange_status get_status() const = 0;
		virtual const std::vector<tradable_pair> get_tradable_pairs() const = 0;
		//virtual const std::unordered_map<tradable_pair, ticker_data> get_ticker_data(const std::vector<tradable_pair>& tradablePairs) const = 0;
		virtual const order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const = 0;
		virtual const std::unordered_map<asset_symbol, double> get_balances() const = 0;
		virtual const std::unordered_map<tradable_pair, double> get_fees(const std::vector<tradable_pair>& tradablePairs) const = 0;
		virtual const std::string add_order(const trade_description& description) = 0;
		//virtual int cancel_order(const std::string& orderId) const = 0;
		//virtual const std::vector<order_description> get_open_orders() const = 0;
		//virtual const std::vector<order_description> get_closed_orders() const = 0;
		virtual websocket_stream& get_websocket_stream() = 0;
	};

	template<typename MarketApi, typename TradeApi>
	class multi_component_exchange final : public exchange
	{
	private:
		std::unique_ptr<MarketApi> _marketApi;
		std::unique_ptr<TradeApi> _tradeApi;

	public:
		multi_component_exchange(exchange_id id, std::unique_ptr<MarketApi> dataApi, std::unique_ptr<TradeApi> tradeApi)
			: exchange{ id }, _marketApi{ std::move(dataApi) }, _tradeApi{ std::move(tradeApi) }
		{}

		exchange_status get_status() const override
		{
			return _marketApi->get_status();
		}

		const std::vector<tradable_pair> get_tradable_pairs() const override
		{
			return _marketApi->get_tradable_pairs();
		}

		const order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const override
		{
			return _marketApi->get_order_book(tradablePair, depth);
		}

		websocket_stream& get_websocket_stream()
		{
			return _marketApi->get_websocket_stream();
		}

		const std::unordered_map<asset_symbol, double> get_balances() const override
		{
			return _tradeApi->get_balances();
		}

		const std::unordered_map<tradable_pair, double> get_fees(const std::vector<tradable_pair>& tradablePairs) const override
		{
			return _tradeApi->get_fees(tradablePairs);
		}

		const std::string add_order(const trade_description& description) override
		{
			return _tradeApi->add_order(description);
		}
	};

	typedef multi_component_exchange<exchange, paper_trader> live_test_exchange;
}