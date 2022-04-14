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
		int _nextOrderNumber;

		bool has_sufficient_funds(const std::string& asset, double amount) const;
		std::string execute_trade(std::string gainedAsset, double gainValue, std::string soldAsset, double soldValue);

	public:
		explicit paper_trade_api(paper_trading_config config);

		double get_fee(const tradable_pair& tradablePair) const;
		unordered_string_map<double> get_balances() const noexcept { return _balances; }
		constexpr std::vector<order_description> get_open_orders() const noexcept { return std::vector<order_description>{}; }
		constexpr std::vector<order_description> get_closed_orders() const noexcept { return std::vector<order_description>{}; }

		std::string add_order(const trade_description& description);
		void cancel_order(std::string_view orderId);
	};
}