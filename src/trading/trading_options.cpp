#include "trading_options.h"

namespace cb
{
	trading_options::trading_options()
		: _maxTradePercent{ 0.0 }, _fiatCurrency{ "" }
	{}

	trading_options::trading_options(double maxTradePercent, asset_symbol fiatCurrency)
		: _maxTradePercent{ maxTradePercent }, _fiatCurrency{ std::move(fiatCurrency) }
	{}
}