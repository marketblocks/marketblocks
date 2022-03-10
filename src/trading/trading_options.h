#pragma once

#include <string>

namespace cb
{
	class trading_options
	{
	private:
		double _maxTradePercent;
		std::string _fiatCurrency;

	public:
		constexpr trading_options()
			: _maxTradePercent{ 0.0 }, _fiatCurrency{ "" }
		{}

		constexpr trading_options(double maxTradePercent, std::string fiatCurrency)
			: _maxTradePercent{ maxTradePercent }, _fiatCurrency{ std::move(fiatCurrency) }
		{}

		constexpr double max_trade_percent() const noexcept { return _maxTradePercent; }
		constexpr const std::string& fiat_currency() const noexcept { return _fiatCurrency; }
	};
}