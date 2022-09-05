#pragma once

#include "trading_constants.h"

namespace mb
{
	class order_confirmation
	{
	private:
		std::string _orderId;
		order_status _orderStatus;
		double _originalQty;
		double _filledQty;
		double _price;

	public:
		order_confirmation(
			std::string orderId,
			order_status orderStatus,
			double originalQty,
			double filledQty,
			double price);

		const std::string& order_id() const noexcept { return _orderId; }
		order_status orderstatus() const noexcept { return _orderStatus; }
		double original_qty() const noexcept { return _originalQty; }
		double filled_qty() const noexcept { return _filledQty; }
		double price() const noexcept { return _price; }
	};
}