#pragma once

#include <unordered_map>
#include <string>

#include "trading/asset_symbol.h"
#include "trading/trading_constants.h"
#include "trading/tradable_pair.h"
#include "trading/trade_description.h"
#include "trading/fee_schedule.h"

namespace cb
{
	class paper_trader
	{
	private:
		fee_schedule _feeSchedule;
		std::unordered_map<asset_symbol, double> _balances;

		bool has_sufficient_funds(const asset_symbol& asset, double amount) const;
		trade_result execute_trade(asset_symbol gainedAsset, double gainValue, asset_symbol soldAsset, double soldValue);

	public:
		explicit paper_trader(fee_schedule feeSchedule, std::unordered_map<asset_symbol, double> initialBalances);

		const std::unordered_map<tradable_pair, double> get_fees(const std::vector<tradable_pair>& tradablePairs) const;
		const std::unordered_map<asset_symbol, double> get_balances() const { return _balances; }

		trade_result trade(const trade_description& description);
	};
}