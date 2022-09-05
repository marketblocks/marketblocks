#pragma once

namespace mb::internal
{
	class binance_order_filters
	{
	private:
		int _pricePrecision;
		int _qtyPrecision;
		double _minQty;
		double _minValue;

	public:
		constexpr binance_order_filters(int pricePrecision, int qtyPrecision, double minQty, double minValue)
			: _pricePrecision{ pricePrecision }, _qtyPrecision{ qtyPrecision }, _minQty{ minQty }, _minValue{ minValue }
		{}

		constexpr binance_order_filters()
			: _pricePrecision{ 2 }, _qtyPrecision{ 4 }, _minQty{ 0.00001 }, _minValue{ 10.0 }
		{}

		constexpr int price_precision() const noexcept { return _pricePrecision; }
		constexpr int qty_precision() const noexcept { return _qtyPrecision; }
		constexpr double min_qty() const noexcept { return _minQty; }
		constexpr double min_value() const noexcept { return _minValue; }
	};
}