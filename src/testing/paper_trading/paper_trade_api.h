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
		double _fee;
		unordered_string_map<double> _balances;
		std::vector<order_description> _closedOrders;
		int _nextOrderNumber;

		bool has_sufficient_funds(const std::string& asset, double amount) const;
		void execute_trade(std::string gainedAsset, double gainValue, std::string soldAsset, double soldValue);
		void record_order_description(const trade_description& tradeDescription, std::string_view orderId);

	public:
		explicit paper_trade_api(paper_trading_config config);

		double get_fee(const tradable_pair& tradablePair) const;
		unordered_string_map<double> get_balances() const noexcept { return _balances; }
		std::vector<order_description> get_open_orders() const noexcept { return std::vector<order_description>{}; }
		const std::vector<order_description>& get_closed_orders() const noexcept { return _closedOrders; }

		std::string add_order(const trade_description& description);
		void cancel_order(std::string_view orderId);
	};
}