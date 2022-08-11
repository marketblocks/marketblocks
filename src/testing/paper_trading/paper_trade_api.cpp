#include <fmt/format.h>

#include "paper_trade_api.h"
#include "common/utils/financeutils.h"
#include "common/utils/containerutils.h"
#include "common/exceptions/mb_exception.h"

namespace
{
	using namespace mb;

	bool should_close_limit_order(const order_request& request, double currentPrice)
	{
		double orderPrice{ request.get(order_request_parameter::ASSET_PRICE) };

		return 
			(request.action() == trade_action::BUY && currentPrice <= orderPrice) ||
			(request.action() == trade_action::SELL && currentPrice >= orderPrice);
	}

	bool should_close_stop_loss_order(const order_request& request, double currentPrice)
	{
		double orderPrice{ request.get(order_request_parameter::STOP_PRICE) };

		return
			(request.action() == trade_action::BUY && currentPrice >= orderPrice) ||
			(request.action() == trade_action::SELL && currentPrice <= orderPrice);
	}

	bool should_close_trailing_stop_loss_order(const order_request& request, double currentPrice, double& maxOrMin)
	{
		double delta{ request.get(order_request_parameter::TRAILING_DELTA) };

		if (request.action() == trade_action::BUY)
		{
			maxOrMin = std::min(maxOrMin, currentPrice);
			return currentPrice >= maxOrMin * (1 + delta);
		}
		else
		{
			maxOrMin = std::max(maxOrMin, currentPrice);
			return currentPrice <= maxOrMin * (1 - delta);
		}
	}

	order_description to_order_description(std::string orderId, double fillPrice, std::time_t time, const order_request& request)
	{
		return order_description{
			time,
			std::move(orderId),
			request.pair().to_string('/'),
			request.action(),
			fillPrice,
			request.get(order_request_parameter::VOLUME) };
	}
}

namespace mb
{
	paper_trade_api::paper_trade_api(
		paper_trading_config config,
		std::string_view exchangeId,
		get_price_function getPrice,
		get_time_function getTime)
		: 
		_getPrice{ std::move(getPrice) },
		_getTime{ std::move(getTime) },
		_exchangeId{ exchangeId }, 
		_fee{ config.fee() },
		_balances{std::move(config.balances())}, 
		_nextOrderNumber{1}
	{}

	bool paper_trade_api::has_sufficient_funds(const std::string& asset, double amount) const
	{
		auto balanceIt = _balances.find(asset);
		if (balanceIt == _balances.end())
		{
			return false;
		}

		return balanceIt->second >= amount;
	}

	void paper_trade_api::execute_order(std::string_view orderId, order_request& request, double fillPrice)
	{
		double volume{ request.get(order_request_parameter::VOLUME) };
		double cost = calculate_cost(fillPrice, volume);
		double fee = cost * _fee * 0.01;
		std::string gainedAsset;
		std::string soldAsset;
		double gainValue;
		double soldValue;

		if (request.action() == trade_action::BUY)
		{
			gainedAsset = request.pair().asset();
			soldAsset = request.pair().price_unit();
			gainValue = volume;
			soldValue =	cost + fee;
		}
		else
		{
			gainedAsset = request.pair().price_unit();
			soldAsset = request.pair().asset();
			gainValue = cost - fee;
			soldValue = volume;
		}

		if (!has_sufficient_funds(soldAsset, soldValue))
		{
			throw mb_exception{ fmt::format("Insufficient funds ({0})", soldAsset) };
		}

		_balances[gainedAsset] += gainValue;
		_balances[soldAsset] -= soldValue;

		_closedOrders.emplace_back(to_order_description(orderId.data(), fillPrice, _getTime(), std::move(request)));
		_openOrders.erase(orderId.data());
	}

	void paper_trade_api::try_fill_open_orders()
	{
		std::vector<std::string> openOrderIds{ to_vector<std::string>(
			_openOrders,
			[](std::pair<std::string, order_request> pair) { return pair.first; }) };

		for (auto& id : openOrderIds)
		{
			try_fill_order(id);
		}
	}

	bool paper_trade_api::try_fill_order(std::string_view orderId)
	{
		order_request& request{ _openOrders.at(orderId.data()) };
		double price{ _getPrice(request.pair()) };
		double fillPrice;
		bool fill = false;

		switch (request.order_type())
		{
			case order_type::MARKET:
			{
				fillPrice = price;
				fill = true;
				break;
			}
			case order_type::LIMIT:
			{
				fill = should_close_limit_order(request, price);
				if (fill)
				{
					fillPrice = request.get(order_request_parameter::ASSET_PRICE);
				}

				break;
			}
			case order_type::STOP_LOSS:
			{
				fill = should_close_stop_loss_order(request, price);
				if (fill)
				{
					fillPrice = request.get(order_request_parameter::STOP_PRICE);
				}

				break;
			}
			case order_type::TRAILING_STOP_LOSS:
			{
				if (_trailingOrderLimits.find(orderId.data()) == _trailingOrderLimits.end())
				{
					_trailingOrderLimits.emplace(orderId.data(), price);
				}

				fill = should_close_trailing_stop_loss_order(request, price, _trailingOrderLimits[orderId.data()]);
				if (fill)
				{
					fillPrice = request.get(order_request_parameter::STOP_PRICE);
					_trailingOrderLimits.erase(orderId.data());
				}

				break;
			}
			default:
			{
				fill = false;
				break;
			}
		}

		if (fill)
		{
			execute_order(orderId.data(), request, fillPrice);
		}

		return fill;
	}

	double paper_trade_api::get_fee(const tradable_pair& tradablePair) const
	{
		return _fee;
	}

	std::unordered_map<std::string,double> paper_trade_api::get_balances() const
	{
		return _balances;
	}

	std::vector<order_description> paper_trade_api::get_open_orders() const
	{
		std::vector<order_description> orders;
		orders.reserve(_openOrders.size());

		for (auto& [orderId, trade] : _openOrders)
		{
			orders.emplace_back(to_order_description(orderId, trade.get(order_request_parameter::ASSET_PRICE), _getTime(), trade));
		}

		return orders;
	}

	std::vector<order_description> paper_trade_api::get_closed_orders() const
	{
		return _closedOrders;
	}

	std::string paper_trade_api::add_order(const order_request& request)
	{
		std::string orderId = std::to_string(_nextOrderNumber++);
		_openOrders.emplace(orderId, request);

		try_fill_order(orderId);
		
		return orderId;
	}

	void paper_trade_api::cancel_order(std::string_view orderId)
	{
		auto it = _openOrders.find(orderId.data());

		if (it == _openOrders.end())
		{
			throw mb_exception{ fmt::format("Could not find order with id = {}", orderId) };
		}

		_openOrders.erase(it);
	}
}