#include <fmt/format.h>

#include "paper_trade_api.h"
#include "common/utils/mathutils.h"
#include "common/utils/financeutils.h"
#include "common/utils/containerutils.h"
#include "common/exceptions/mb_exception.h"

namespace
{
	using namespace mb;

	static constexpr int VolumePrecision = 10;
	static double VolumeModifier = std::pow(10, VolumePrecision);

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

	volume_t to_integer_volume(double volume)
	{
		return static_cast<volume_t>(volume * VolumeModifier);
	}

	std::unordered_map<std::string, volume_t> create_initialised_balances(const std::unordered_map<std::string, double>& initialBalances)
	{
		return to_unordered_map<std::string, volume_t>(
			initialBalances,
			[](const std::pair<std::string, double>& pair) { return pair.first; },
			[](const std::pair<std::string, double>& pair) { return to_integer_volume(pair.second); });
	}
}

namespace mb
{
	paper_trade_api::paper_trade_api(
		paper_trading_config config,
		std::shared_ptr<websocket_stream> websocketStream,
		std::string_view exchangeId,
		get_time_function getTime)
		:
		_websocketStream{ std::move(websocketStream) },
		_getTime{ std::move(getTime) },
		_exchangeId{ exchangeId },
		_fee{ config.fee() },
		_balances{ create_initialised_balances(config.balances()) },
		_nextOrderNumber{ 1 },
		_tradingMutex{}
	{
		_websocketStream->add_trade_update_handler([this](trade_update_message message) { trade_update_handler(std::move(message)); });
	}

	bool paper_trade_api::has_sufficient_funds(const std::string& asset, volume_t amount) const
	{
		auto balanceIt = _balances.find(asset);
		if (balanceIt == _balances.end())
		{
			return false;
		}

		return balanceIt->second >= amount;
	}

	bool paper_trade_api::try_fill_order(std::string_view orderId)
	{
		order_request& request{ _openOrders.at(orderId.data()) };
		double price{ _websocketStream->get_last_trade(request.pair()).price() };

		if (price == 0.0)
		{
			return false;
		}

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
				fillPrice = price;
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

		volume_t gainVolume{ to_integer_volume(gainValue) };
		volume_t soldVolume{ to_integer_volume(soldValue) };

		if (!has_sufficient_funds(soldAsset, soldVolume))
		{
			throw mb_exception{ fmt::format("Insufficient funds ({0})", soldAsset) };
		}

		_balances[gainedAsset] += gainVolume;
		_balances[soldAsset] -= soldVolume;

		_closedOrders.emplace_back(to_order_description(orderId.data(), fillPrice, _getTime(), std::move(request)));
		_openOrders.erase(orderId.data());
	}

	void paper_trade_api::trade_update_handler(trade_update_message message)
	{
		std::lock_guard lock{ _tradingMutex };

		std::vector<std::string>& orderIds{ _openOrdersByPair[message.pair()] };
		size_t orderCount = orderIds.size();
		int index = 0;
		
		for (int i = 0; i < orderCount; ++i)
		{
			auto it = orderIds.begin() + index;

			if (!contains(_openOrders, *it) || try_fill_order(*it))
			{
				orderIds.erase(it);
			}
			else
			{
				++index;
			}
		}
	}

	double paper_trade_api::get_fee(const tradable_pair& tradablePair) const
	{
		return _fee;
	}

	std::unordered_map<std::string,double> paper_trade_api::get_balances() const
	{
		std::lock_guard lock{ _tradingMutex };
		return to_unordered_map<std::string, double>(
			_balances,
			[](const std::pair<std::string, volume_t>& pair) { return pair.first; },
			[](const std::pair<std::string, volume_t>& pair) { return static_cast<double>(pair.second) / VolumeModifier; });
	}

	std::vector<order_description> paper_trade_api::get_open_orders() const
	{
		std::lock_guard lock{ _tradingMutex };

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
		std::lock_guard lock{ _tradingMutex };
		return _closedOrders;
	}

	std::string paper_trade_api::add_order(const order_request& request)
	{
		std::lock_guard lock{ _tradingMutex };

		std::string orderId{ std::to_string(_nextOrderNumber++) };
		_openOrders.emplace(orderId, request);
		_openOrdersByPair[request.pair()].emplace_back(orderId);
		
		if (_websocketStream->get_subscription_status(unique_websocket_subscription::create_trade_sub(request.pair())) == subscription_status::UNSUBSCRIBED)
		{
			_websocketStream->subscribe(websocket_subscription::create_trade_sub({ request.pair() }));
		}

		try_fill_order(orderId);
		
		return orderId;
	}

	void paper_trade_api::cancel_order(std::string_view orderId)
	{
		std::lock_guard lock{ _tradingMutex };

		auto it = _openOrders.find(orderId.data());

		if (it == _openOrders.end())
		{
			throw mb_exception{ fmt::format("Could not find order with id = {}", orderId) };
		}

		_openOrders.erase(it);
	}

	order_status paper_trade_api::get_order_status(std::string_view orderId) const
	{
		std::lock_guard lock{ _tradingMutex };

		if (contains(_openOrders, orderId.data()))
		{
			return order_status::OPEN;
		}

		return order_status::CLOSED;
	}
}