#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "trading/tradable_pair.h"
#include "trading/asset_symbol.h"
#include "trading/trading_constants.h"
#include "trading/order_book.h"
#include "trading/trade_description.h"
#include "paper_trading/paper_trader.h"

#include "exchange_id.h"
#include "exchanges/websockets/websocket_stream.h"

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

		virtual const std::vector<tradable_pair> get_tradable_pairs() const = 0;
		virtual const std::unordered_map<tradable_pair, order_book_state> get_order_book(const std::vector<tradable_pair>& tradablePairs, int depth) const = 0;
		virtual const std::unordered_map<asset_symbol, double> get_balances() const = 0;
		virtual const std::unordered_map<tradable_pair, double> get_fees(const std::vector<tradable_pair>& tradablePairs) const = 0;
		virtual trade_result trade(const trade_description& description) = 0;

		virtual websocket_stream& get_or_connect_websocket() = 0;
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

		const std::vector<tradable_pair> get_tradable_pairs() const override
		{
			return _marketApi->get_tradable_pairs();
		}

		const std::unordered_map<tradable_pair, order_book_state> get_order_book(const std::vector<tradable_pair>& tradablePairs, int depth) const override
		{
			return _marketApi->get_order_book(tradablePairs, depth);
		}

		websocket_stream& get_or_connect_websocket()
		{
			return _marketApi->get_or_connect_websocket();
		}

		const std::unordered_map<asset_symbol, double> get_balances() const override
		{
			return _tradeApi->get_balances();
		}

		const std::unordered_map<tradable_pair, double> get_fees(const std::vector<tradable_pair>& tradablePairs) const override
		{
			return _tradeApi->get_fees(tradablePairs);
		}

		trade_result trade(const trade_description& description) override
		{
			return _tradeApi->trade(description);
		}
	};

	typedef multi_component_exchange<exchange, paper_trader> live_test_exchange;
}