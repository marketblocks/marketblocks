#pragma once

class PriceData
{
private:
	double _bid;
	double _ask;

public:
	explicit PriceData(double bid, double ask)
		: _bid{bid}, _ask{ask}
	{}

	double bid() const { return _bid; }
	double ask() const { return _ask; }
};