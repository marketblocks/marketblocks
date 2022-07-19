#pragma once

#include "template_config.h"
#include "exchanges/exchange.h"
#include "networking/http/http_service.h"

namespace mb
{
	class template_api : public exchange
	{
	private:
		std::unique_ptr<http_service> _httpService;

	public:
		template_api(
			template_config config,
			std::unique_ptr<http_service> httpService,
			std::shared_ptr<websocket_stream> websocketStream,
			bool enableTesting = false);

		exchange_status get_status() const override;
		std::vector<tradable_pair> get_tradable_pairs() const override;
		std::vector<ohlcv_data> get_ohlcv(const tradable_pair& tradablePair, ohlcv_interval interval, int count) const override;
		double get_price(const tradable_pair& tradablePair) const override;
		order_book_state get_order_book(const tradable_pair& tradablePair, int depth) const override;
		double get_fee(const tradable_pair& tradablePair) const override;
		unordered_string_map<double> get_balances() const override;
		std::vector<order_description> get_open_orders() const override;
		std::vector<order_description> get_closed_orders() const override;
		std::string add_order(const order_request& description) override;
		void cancel_order(std::string_view orderId) override;
	};

	template<>
	std::unique_ptr<exchange> create_exchange_api<template_api>(bool testing);
}