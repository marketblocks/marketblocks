#pragma once

#include "order_request.h"
#include "common/csv/csv.h"

namespace mb
{
	class order_description
	{
	private:
		std::time_t _timeStamp;
		std::string _orderId;
		order_type _orderType;
		std::string _pairName;
		trade_action _action;
		double _price;
		double _volume;

	public:
		order_description(
			std::time_t timeStamp,
			std::string orderId,
			order_type orderType,
			std::string pairName,
			trade_action action,
			double price,
			double volume);

		std::time_t time_stamp() const noexcept { return _timeStamp; }
		const std::string& order_id() const noexcept { return _orderId; }
		order_type ordertype() const noexcept { return _orderType; }
		const std::string& pair_name() const noexcept { return _pairName; }
		trade_action action() const noexcept { return _action; }
		double price() const noexcept { return _price; }
		double volume() const noexcept { return _volume; }
	};

	template<>
	csv_row to_csv_row(const order_description& data);
}
