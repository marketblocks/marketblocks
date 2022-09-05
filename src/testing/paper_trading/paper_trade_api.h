#pragma once

#include <unordered_map>
#include <string>

#include "paper_trading_config.h"
#include "exchanges/exchange.h"
#include "common/file/config_file_reader.h"
#include "trading/trading_constants.h"
#include "trading/tradable_pair.h"
#include "trading/order_request.h"
#include "trading/order_description.h"
#include "common/utils/timeutils.h"
#include "common/types/concurrent_wrapper.h"

namespace mb
{
	using volume_t = unsigned long long;

	class paper_trade_api : public trade_api
	{
	private:
		using get_time_function = std::function<std::time_t()>;

		std::shared_ptr<websocket_stream> _websocketStream;
		get_time_function _getTime;
		std::string_view _exchangeId;
		double _fee;
		std::unordered_map<std::string, double> _trailingOrderLimits;
		std::unordered_map<std::string, volume_t> _balances;
		std::unordered_map<std::string, order_request> _openOrders;
		std::unordered_map<tradable_pair, std::vector<std::string>> _openOrdersByPair;
		std::vector<order_description> _closedOrders;
		int _nextOrderNumber;
		mutable std::mutex _tradingMutex;

		bool has_sufficient_funds(const std::string& asset, volume_t amount) const;
		bool try_fill_order(std::string_view orderId);
		void execute_order(std::string_view orderId, order_request& request, double fillPrice);
		void trade_update_handler(trade_update_message message);

	public:
		explicit paper_trade_api(
			paper_trading_config config,
			std::shared_ptr<websocket_stream> websocketStream,
			std::string_view exchangeId,
			get_time_function getTime);

		std::string_view exchange_id() const noexcept { return _exchangeId; }

		double get_fee(const tradable_pair& tradablePair) const override;
		std::unordered_map<std::string,double> get_balances() const override;
		std::vector<order_description> get_open_orders() const override;
		std::vector<order_description> get_closed_orders() const override;
		std::string add_order(const order_request& description) override;
		order_confirmation add_order_confirm(const order_request& description) override;
		void cancel_order(std::string_view orderId) override;

		order_status get_order_status(std::string_view orderId) const;
	};

	template<typename GetTime>
	std::shared_ptr<paper_trade_api> create_paper_trade_api(std::string_view id, std::shared_ptr<websocket_stream> websocketStream, GetTime getTime)
	{
		paper_trading_config config{ internal::load_or_create_config<paper_trading_config>() };

		return std::make_unique<paper_trade_api>(
			std::move(config),
			std::move(websocketStream),
			id,
			std::move(getTime));
	}
}