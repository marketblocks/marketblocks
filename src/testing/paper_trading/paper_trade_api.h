#pragma once

#include <unordered_map>
#include <string>

#include "paper_trading_config.h"
#include "trading/trading_constants.h"
#include "trading/tradable_pair.h"
#include "trading/trade_description.h"
#include "trading/order_description.h"

namespace mb
{
	class paper_trade_api
	{
	private:
		using get_price_function = std::function<double(const tradable_pair&)>;

		get_price_function _getPrice;
		std::string_view _exchangeId;
		double _fee;
		unordered_string_map<double> _balances;
		unordered_string_map<trade_description> _openTrades;
		std::vector<order_description> _closedOrders;
		int _nextOrderNumber;

		bool has_sufficient_funds(const std::string& asset, double amount) const;
		void execute_order(std::string orderId, const trade_description& description, double fillPrice);

	public:
		explicit paper_trade_api(
			paper_trading_config config,
			std::string_view exchangeId,
			get_price_function getPrice);

		void fill_open_orders();

		std::string_view exchange_id() const noexcept { return _exchangeId; }

		double get_fee(const tradable_pair& tradablePair) const;
		unordered_string_map<double> get_balances() const noexcept { return _balances; }
		std::vector<order_description> get_open_orders() const;
		std::vector<order_description> get_closed_orders() const noexcept { return _closedOrders; }

		std::string add_order(const trade_description& description);
		void cancel_order(std::string_view orderId);
	};
}