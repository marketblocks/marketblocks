#include "trade_description.h"
#include "common/utils/financeutils.h"

namespace cb
{
	trade_description::trade_description(
		cb::order_type orderType,
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

	trade_description create_trade_by_cost(tradable_pair pair, trade_action action, double assetPrice, double tradeCost)
	{
		return trade_description{ order_type::LIMIT, std::move(pair), action, assetPrice, calculate_volume(assetPrice, tradeCost) };
	}
}