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
		trading_options();
		explicit trading_options(double maxTradePercent, asset_symbol fiatCurrency);

		double max_trade_percent() const { return _maxTradePercent; }
		const asset_symbol& fiat_currency() const { return _fiatCurrency; }
	};
}