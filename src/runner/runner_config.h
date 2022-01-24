#pragma once

#include <vector>

#include "trading/asset_symbol.h"
#include "trading/trading_options.h"
#include "common/file/json.h"

namespace cb
{
	enum class run_mode
	{
		LIVE, LIVETEST, BACKTEST
	};

	class runner_config
	{
	private:
		static const std::vector<std::string> DEFAULT_EXCHANGE_IDS;
		static const double DEFAULT_TRADE_PERCENT;
		static const asset_symbol DEFAULT_FIAT_CURRENCY;

		std::vector<std::string> _exchangeIds;
		double _tradePercent;
		asset_symbol _fiatCurrency;

		void validate();

	public:
		runner_config();
		explicit runner_config(
			std::vector<std::string> exchangeIds,
			double tradePercent,
			asset_symbol fiatCurrency);

		static std::string name() { return "runnerConfig"; }
		
		const std::vector<std::string>& exchange_ids() const { return _exchangeIds; }
		double trade_percent() const { return _tradePercent; }
		const asset_symbol& fiat_currency() const { return _fiatCurrency; }

		trading_options get_trading_options() const;
	};

	template<>
	runner_config from_json(const json_document& json);

	template<>
	void to_json(const runner_config& config, json_writer& writer);
}