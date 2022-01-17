#pragma once

#include <vector>

#include "trading/asset_symbol.h"
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

	public:
		explicit runner_config(std::vector<std::string> exchangeIds);

		static runner_config create_default();
		static std::string name() { return "runnerConfig"; }
		static runner_config deserialize(json_wrapper& json);
		std::string serialize() const;

		const std::vector<std::string>& exchange_ids() const { return _exchangeIds; }

	};

	class trading_options
	{
	private:
		double _maxTradePercent;
		asset_symbol _fiatCurrency;

	public:
		trading_options();
		explicit trading_options(double maxTradePercent, asset_symbol fiatCurrency);

		static trading_options create_default();
		static std::string name() { return "tradingOptions"; }
		static trading_options deserialize(json_wrapper& json);
		std::string serialize() const;

		double max_trade_percent() const { return _maxTradePercent; }
		const asset_symbol& fiat_currency() const { return _fiatCurrency; }

	};
}