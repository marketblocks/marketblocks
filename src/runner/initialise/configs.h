#pragma once

#include <vector>

#include "trading/asset_symbol.h"
#include "trading/trading_options.h"
#include "common/file/json_wrapper.h"

namespace cb
{
	enum class run_mode
	{
		LIVE, LIVETEST, BACKTEST
	};

	class runner_config
	{
	private:
		std::vector<std::string> _exchangeIds;
		double _tradePercent;
		asset_symbol _fiatCurrency;

		static std::vector<std::string> default_exchange_ids() { return std::vector<std::string>{}; }
		static double default_trade_percent() { return 0.05; }
		static asset_symbol default_fiat_currency() { return asset_symbol{ "GBP" }; }

	public:
		explicit runner_config(
			std::vector<std::string> exchangeIds,
			double tradePercent,
			asset_symbol fiatCurrency);

		static runner_config create_default();
		static std::string name() { return "runnerConfig"; }
		static runner_config deserialize(json_wrapper& json);
		std::string serialize() const;

		const std::vector<std::string>& exchange_ids() const { return _exchangeIds; }
		double max_trade_percent() const { return _tradePercent; }
		const asset_symbol& fiat_currency() const { return _fiatCurrency; }

		trading_options get_trading_options() const;
	};
}