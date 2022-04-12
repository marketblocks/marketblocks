#include "paper_trader.h"
#include "common/utils/financeutils.h"
#include "common/utils/containerutils.h"
#include "common/exceptions/cb_exception.h"

namespace mb
{
	paper_trader::paper_trader(paper_trading_config config)
		: _fee{ config.fee() }, _balances{ std::move(config.balances()) }, _nextOrderNumber{ 1 }
	{}

	bool paper_trader::has_sufficient_funds(const std::string& asset, double amount) const
	{
		auto balanceIt = _balances.find(asset);
		if (balanceIt == _balances.end())
		{
			return false;
		}

		return balanceIt->second >= amount;
	}

	std::string paper_trader::execute_trade(std::string gainedAsset, double gainValue, std::string soldAsset, double soldValue)
	{
		if (!has_sufficient_funds(soldAsset, soldValue))
		{
			throw cb_exception{ "Insufficient funds" };
		}

		_balances[gainedAsset] += gainValue;
		_balances[soldAsset] -= soldValue;

		return std::to_string(_nextOrderNumber++);
	}

	double paper_trader::get_fee(const tradable_pair& tradablePair) const
	{
		return _fee;
	}

	std::string paper_trader::add_order(const trade_description& description)
	{
		double cost = calculate_cost(description.asset_price(), description.volume());
		double fee = cost * _fee * 0.01;

		if (description.action() == trade_action::BUY)
		{
			return execute_trade(
				description.pair().asset(),
				description.volume(),
				description.pair().price_unit(),
				cost + fee);
		}
		else
		{
			return execute_trade(
				description.pair().price_unit(),
				cost - fee,
				description.pair().asset(),
				description.volume());
		}
	}

	void paper_trader::cancel_order(std::string_view orderId)
	{}
}