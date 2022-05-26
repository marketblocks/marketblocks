#include "paper_trade_api.h"
#include "common/utils/financeutils.h"
#include "common/utils/containerutils.h"
#include "common/exceptions/mb_exception.h"

namespace mb
{
	paper_trade_api::paper_trade_api(paper_trading_config config, std::string_view exchangeId)
		: _exchangeId{ exchangeId }, _fee{ config.fee() }, _balances{std::move(config.balances())}, _nextOrderNumber{1}
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

	void paper_trade_api::execute_trade(std::string gainedAsset, double gainValue, std::string soldAsset, double soldValue)
	{
		if (!has_sufficient_funds(soldAsset, soldValue))
		{
			throw mb_exception{ std::format("Insufficient funds ({0})", soldAsset) };
		}

		_balances[gainedAsset] += gainValue;
		_balances[soldAsset] -= soldValue;
	}

	void paper_trade_api::record_order_description(const trade_description& tradeDescription, std::string_view orderId)
	{
		_closedOrders.emplace_back(
			std::string{ orderId },
			tradeDescription.pair().to_string('/'),
			tradeDescription.action(),
			tradeDescription.asset_price(),
			tradeDescription.volume());
	}

	double paper_trade_api::get_fee(const tradable_pair& tradablePair) const
	{
		return _fee;
	}

	std::string paper_trade_api::add_order(const trade_description& description)
	{
		double cost = calculate_cost(description.asset_price(), description.volume());
		double fee = cost * _fee * 0.01;

		if (description.action() == trade_action::BUY)
		{
			execute_trade(
				description.pair().asset(),
				description.volume(),
				description.pair().price_unit(),
				cost + fee);
		}
		else
		{
			execute_trade(
				description.pair().price_unit(),
				cost - fee,
				description.pair().asset(),
				description.volume());
		}

		std::string orderId = std::to_string(_nextOrderNumber++);
		record_order_description(description, orderId);

		return orderId;
	}

	void paper_trade_api::cancel_order(std::string_view orderId)
	{}
}