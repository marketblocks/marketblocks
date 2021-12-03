class TradingOptions
{
private:
	double _maxTradePercent;

public:
	explicit TradingOptions(double maxTradePercent)
		: _maxTradePercent{ maxTradePercent }
	{
	}

	double max_trade_percent() const { return _maxTradePercent; }
};