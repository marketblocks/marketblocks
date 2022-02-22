#include "order_description.h"

namespace cb
{
	order_description::order_description(std::string orderId, std::string pairName, trade_action action, double price, double volume)
		: _orderId{ std::move(orderId) }, _pairName{ std::move(pairName) }, _action{ action }, _price{ price }, _volume{ volume }
	{}
}