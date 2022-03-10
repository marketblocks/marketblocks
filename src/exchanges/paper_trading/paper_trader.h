#pragma once

#include <unordered_map>
#include <string>

#include "trading/trading_constants.h"
#include "trading/tradable_pair.h"
#include "trading/trade_description.h"
#include "trading/fee_schedule.h"
#include "trading/order_description.h"
#include "common/types/unordered_string_map.h"

namespace cb
{
	class paper_trader
	{
	private:
		fee_schedule _feeSchedule;
		unordered_string_map<double> _balances;

		bool has_sufficient_funds(const std::string& asset, double amount) const;
		std::string execute_trade(std::string gainedAsset, double gainValue, std::string soldAsset, double soldValue);

	public:
		explicit paper_trader(fee_schedule feeSchedule, unordered_string_map<double> initialBalances);

		double get_fee(const tradable_pair& tradablePair) const;
		unordered_string_map<double> get_balances() const noexcept { return _balances; }
		constexpr std::vector<order_description> get_open_orders() const noexcept { return std::vector<order_description>{}; }
		constexpr std::vector<order_description> get_closed_orders() const noexcept { return std::vector<order_description>{}; }

		std::string add_order(const trade_description& description);
		void cancel_order(std::string_view orderId);
	};
}