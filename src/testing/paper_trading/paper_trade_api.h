#pragma once

#include <unordered_map>
#include <string>

#include "paper_trading_config.h"
#include "exchanges/exchange.h"
#include "common/file/config_file_reader.h"
#include "trading/trading_constants.h"
#include "trading/tradable_pair.h"
#include "trading/trade_description.h"
#include "trading/order_description.h"
#include "common/utils/timeutils.h"

namespace mb
{
	class paper_trade_api : public trade_api
	{
	private:
		using get_price_function = std::function<double(const tradable_pair&)>;
		using get_time_function = std::function<std::time_t()>;

		get_price_function _getPrice;
		get_time_function _getTime;
		std::string_view _exchangeId;
		double _fee;
		unordered_string_map<double> _balances;
		unordered_string_map<trade_description> _openTrades;
		std::vector<order_description> _closedOrders;
		int _nextOrderNumber;

		bool has_sufficient_funds(const std::string& asset, double amount) const;
		void execute_order(std::string orderId, const trade_description& description, double fillPrice);

	public:
		explicit paper_trade_api(
			paper_trading_config config,
			std::string_view exchangeId,
			get_price_function getPrice,
			get_time_function getTime);

		void fill_open_orders();

		std::string_view exchange_id() const noexcept { return _exchangeId; }

		double get_fee(const tradable_pair& tradablePair) const override;
		unordered_string_map<double> get_balances() const override;
		std::vector<order_description> get_open_orders() const override;
		std::vector<order_description> get_closed_orders() const override;
		std::string add_order(const trade_description& description) override;
		void cancel_order(std::string_view orderId) override;
	};

	template<typename GetPrice, typename GetTime>
	std::shared_ptr<paper_trade_api> create_paper_trade_api(std::string_view id, GetPrice getPrice, GetTime getTime)
	{
		paper_trading_config config{ internal::load_or_create_config<paper_trading_config>() };

		return std::make_unique<paper_trade_api>(
			std::move(config),
			id,
			std::move(getPrice),
			std::move(getTime));
	}
}