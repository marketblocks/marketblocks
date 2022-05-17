#pragma once

#include "trade_description.h"
#include "common/csv/csv.h"

namespace mb
{
	class order_description
	{
	private:
		std::string _orderId;
		std::string _pairName;
		trade_action _action;
		double _price;
		double _volume;

	public:
		constexpr order_description(std::string orderId, std::string pairName, trade_action action, double price, double volume)
			: _orderId{ std::move(orderId) }, _pairName{ std::move(pairName) }, _action{ action }, _price{ price }, _volume{ volume }
		{}

		constexpr const std::string& order_id() const noexcept { return _orderId; }
		constexpr const std::string& pair_name() const noexcept { return _pairName; }
		constexpr trade_action action() const noexcept { return _action; }
		constexpr double price() const noexcept { return _price; }
		constexpr double volume() const noexcept { return _volume; }
	};

	template<>
	csv_row to_csv_row(const order_description& data);
}
