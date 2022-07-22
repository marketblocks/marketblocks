#include <fmt/format.h>

#include "paper_trade_api.h"
#include "common/utils/financeutils.h"
#include "common/utils/containerutils.h"
#include "common/exceptions/mb_exception.h"

namespace
{
	using namespace mb;

	bool should_close_limit_order(const order_request& trade, double currentPrice)
	{
		return 
			(trade.action() == trade_action::BUY && currentPrice <= trade.asset_price()) ||
			(trade.action() == trade_action::SELL && currentPrice >= trade.asset_price());
	}

	order_description to_order_description(std::string orderId, double fillPrice, std::time_t time, const order_request& trade)
	{
		return order_description{
			time,
			std::move(orderId),
			trade.pair().to_string('/'),
			trade.action(),
			fillPrice,
			trade.volume() };
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

	void paper_trade_api::execute_order(std::string orderId, const order_request& description, double fillPrice)
	{
		double cost = calculate_cost(fillPrice, description.volume());
		double fee = cost * _fee * 0.01;
		std::string gainedAsset;
		std::string soldAsset;
		double gainValue;
		double soldValue;

		if (description.action() == trade_action::BUY)
		{
			gainedAsset = description.pair().asset();
			soldAsset = description.pair().price_unit();
			gainValue = description.volume();
			soldValue =	cost + fee;
		}
		else
		{
			gainedAsset = description.pair().price_unit();
			soldAsset = description.pair().asset();
			gainValue = cost - fee;
			soldValue = description.volume();
		}

		if (!has_sufficient_funds(soldAsset, soldValue))
		{
			throw mb_exception{ fmt::format("Insufficient funds ({0})", soldAsset) };
		}

		_balances[gainedAsset] += gainValue;
		_balances[soldAsset] -= soldValue;

		_closedOrders.emplace_back(to_order_description(std::move(orderId), fillPrice, _getTime(), description));
	}

	void paper_trade_api::fill_open_orders()
	{
		std::unordered_map<tradable_pair, double> prices;
		std::vector<std::string> closedOrders;

		for (auto& [orderId, trade] : _openTrades)
		{
			if (!contains(prices, trade.pair()))
			{
				prices[trade.pair()] = _getPrice(trade.pair());
			}

			double price = prices[trade.pair()];

			if (should_close_limit_order(trade, price))
			{
				execute_order(orderId, trade, trade.asset_price());
				closedOrders.emplace_back(orderId);
			}
		}

		for (auto& id : closedOrders)
		{
			_openTrades.erase(id);
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
		orders.reserve(_openTrades.size());

		for (auto& [orderId, trade] : _openTrades)
		{
			orders.emplace_back(to_order_description(orderId, trade.asset_price(), _getTime(), trade));
		}

		return orders;
	}

	std::vector<order_description> paper_trade_api::get_closed_orders() const
	{
		return _closedOrders;
	}

	std::string paper_trade_api::add_order(const order_request& description)
	{
		std::string orderId = std::to_string(_nextOrderNumber++);
		double price = _getPrice(description.pair());
		
		if (description.order_type() == order_type::MARKET)
		{
			execute_order(orderId, description, price);
		}
		else
		{
			if (should_close_limit_order(description, price))
			{
				execute_order(orderId, description, description.asset_price());
			}
			else
			{
				_openTrades.emplace(orderId, description);
			}
		}
		
		return orderId;
	}

	void paper_trade_api::cancel_order(std::string_view orderId)
	{
		auto it = _openTrades.find(orderId.data());

		if (it == _openTrades.end())
		{
			throw mb_exception{ fmt::format("Could not find order with id = {}", orderId) };
		}

		_openTrades.erase(it);
	}
}