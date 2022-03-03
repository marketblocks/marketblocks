#pragma once

#include "asset_symbol.h"

namespace cb
{
	class trading_options
	{
	private:
		double _maxTradePercent;
		asset_symbol _fiatCurrency;

	public:
		constexpr trading_options()
			: _maxTradePercent{ 0.0 }, _fiatCurrency{ "" }
		{}

		constexpr trading_options(double maxTradePercent, asset_symbol fiatCurrency)
			: _maxTradePercent{ maxTradePercent }, _fiatCurrency{ std::move(fiatCurrency) }
		{}

		constexpr double max_trade_percent() const noexcept { return _maxTradePercent; }
		constexpr const asset_symbol& fiat_currency() const noexcept { return _fiatCurrency; }
	};
}