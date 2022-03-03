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
		static constexpr double DEFAULT_TRADE_PERCENT = 0.05;

		std::vector<std::string> _exchangeIds;
		double _tradePercent;
		asset_symbol _fiatCurrency;

		void validate();

	public:
		runner_config();

		runner_config(
			std::vector<std::string> exchangeIds,
			double tradePercent,
			asset_symbol fiatCurrency);
			
		static constexpr std::string name() noexcept { return "runnerConfig"; }
		
		constexpr const std::vector<std::string>& exchange_ids() const noexcept { return _exchangeIds; }
		constexpr double trade_percent() const noexcept { return _tradePercent; }
		constexpr const asset_symbol& fiat_currency() const noexcept { return _fiatCurrency; }
	};

	template<>
	runner_config from_json(const json_document& json);

	template<>
	void to_json(const runner_config& config, json_writer& writer);
}