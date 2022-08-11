#include "order_request.h"

#include "common/utils/containerutils.h"

namespace mb
{
	order_request::order_request(
		mb::order_type orderType,
		tradable_pair pair,
		trade_action action,
		std::unordered_map<order_request_parameter, double> parameters)
		:
		_orderType{ orderType },
		_pair{ std::move(pair) },
		_action{ std::move(action) },
		_parameters{ std::move(parameters) }
	{}

	double order_request::get(order_request_parameter parameter) const
	{
		return find_or_default<double>(_parameters, parameter);
	}

	order_request create_limit_order(tradable_pair pair, trade_action action, double assetPrice, double volume)
	{
		std::unordered_map<order_request_parameter, double> parameters
		{
			{ order_request_parameter::ASSET_PRICE, assetPrice },
			{ order_request_parameter::VOLUME, volume }
		};

		return order_request{ order_type::LIMIT, std::move(pair), action, std::move(parameters) };
	}

	order_request create_market_order(tradable_pair pair, trade_action action, double volume)
	{
		std::unordered_map<order_request_parameter, double> parameters
		{
			{ order_request_parameter::VOLUME, volume }
		};

		return order_request{ order_type::MARKET, std::move(pair), action, std::move(parameters) };
	}

	order_request create_stop_loss_order(tradable_pair pair, trade_action action, double stopPrice, double volume)
	{
		std::unordered_map<order_request_parameter, double> parameters
		{
			{ order_request_parameter::VOLUME, volume },
			{ order_request_parameter::STOP_PRICE, stopPrice },
		};

		return order_request{ order_type::STOP_LOSS, std::move(pair), action, std::move(parameters) };
	}

	order_request create_trailing_stop_loss_order(tradable_pair pair, trade_action action, double delta, double volume)
	{
		std::unordered_map<order_request_parameter, double> parameters
		{
			{ order_request_parameter::VOLUME, volume },
			{ order_request_parameter::TRAILING_DELTA, delta },
		};

		return order_request{ order_type::TRAILING_STOP_LOSS, std::move(pair), action, std::move(parameters) };
	}
}