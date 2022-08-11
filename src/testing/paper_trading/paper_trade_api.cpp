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
		double price{ request.get(order_request_parameter::ASSET_PRICE) };

		return 
			(request.action() == trade_action::BUY && currentPrice <= price) ||
			(request.action() == trade_action::SELL && currentPrice >= price);
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

	void paper_trade_api::execute_order(std::string orderId, const order_request& request, double fillPrice)
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

		_closedOrders.emplace_back(to_order_description(std::move(orderId), fillPrice, _getTime(), request));
	}

	void paper_trade_api::fill_open_orders()
	{
		std::unordered_map<tradable_pair, double> prices;
		std::vector<std::string> closedOrders;

		for (auto& [orderId, request] : _openOrders)
		{
			if (!contains(prices, request.pair()))
			{
				prices[request.pair()] = _getPrice(request.pair());
			}

			double price = prices[request.pair()];

			if (should_close_limit_order(request, price))
			{
				execute_order(orderId, request, request.get(order_request_parameter::ASSET_PRICE));
				closedOrders.emplace_back(orderId);
			}
		}

		for (auto& id : closedOrders)
		{
			_openOrders.erase(id);
		}
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
		double price = _getPrice(request.pair());
		
		if (request.order_type() == order_type::MARKET)
		{
			execute_order(orderId, request, price);
		}
		else
		{
			if (should_close_limit_order(request, price))
			{
				execute_order(orderId, request, request.get(order_request_parameter::ASSET_PRICE));
			}
			else
			{
				_openOrders.emplace(orderId, request);
			}
		}
		
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