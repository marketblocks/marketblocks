#include "order_request.h"

namespace mb
{
	order_request::order_request(
		mb::order_type orderType,
		tradable_pair pair,
		trade_action action,
		double assetPrice,
		double volume)
		:
		_orderType{ orderType },
		_pair{ std::move(pair) },
		_action{ std::move(action) },
		_assetPrice{ assetPrice },
		_volume{ volume }
	{}

	order_request create_order_by_cost(tradable_pair pair, trade_action action, double assetPrice, double tradeCost)
	{
		return order_request{ order_type::LIMIT, std::move(pair), action, assetPrice, calculate_volume(assetPrice, tradeCost) };
	}
}