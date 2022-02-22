#pragma once

#include "trade_description.h"

namespace cb
{
	class order_description
	{
	private:
		std::string _orderId;
		std::string _pairName;
		trade_action _action;
		double _price;
		double _volume;

	public:
		order_description(std::string orderId, std::string pairName, trade_action action, double price, double volume);

		const std::string& order_id() const { return _orderId; }
		const std::string& pair_name() const { return _pairName; }
		trade_action action() const { return _action; }
		double price() const { return _price; }
		double volume() const { return _volume; }
	};
}
