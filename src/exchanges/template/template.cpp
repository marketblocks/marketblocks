#include "template.h"
#include "template_results.h"
#include "exchanges/exchange_ids.h"
#include "common/file/config_file_reader.h"

namespace mb
{
	template_api::template_api(
		template_config config,
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream,
		bool enableTesting)
		: 
		exchange{ exchange_ids::BYBIT, websocketStream },
		_httpService{ std::move(httpService) }
	{}

	exchange_status template_api::get_status() const
	{
		return mb::read_template::read_system_status("").value();
	}

	std::vector<tradable_pair> template_api::get_tradable_pairs() const
	{
		return mb::read_template::read_tradable_pairs("").value();
	}

	std::vector<ohlcv_data> template_api::get_ohlcv(const tradable_pair& tradablePair, ohlcv_interval interval, int count) const
	{
		return mb::read_template::read_ohlcv("").value();
	}

	double template_api::get_price(const tradable_pair& tradablePair) const
	{
		return mb::read_template::read_price("").value();
	}

	order_book_state template_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		return mb::read_template::read_order_book("").value();
	}

	double template_api::get_fee(const tradable_pair& tradablePair) const
	{
		return mb::read_template::read_fee("").value();
	}

	std::unordered_map<std::string,double> template_api::get_balances() const
	{
		return mb::read_template::read_balances("").value();
	}

	std::vector<order_description> template_api::get_open_orders() const
	{
		return mb::read_template::read_open_orders("").value();
	}

	std::vector<order_description> template_api::get_closed_orders() const
	{
		return mb::read_template::read_closed_orders("").value();
	}

	std::string template_api::add_order(const order_request& description)
	{
		return mb::read_template::read_add_order("").value();
	}

	void template_api::cancel_order(std::string_view orderId)
	{
		mb::read_template::read_cancel_order("").value();
	}

	template<>
	std::unique_ptr<exchange> create_exchange_api<template_api>(bool testing)
	{
		return std::make_unique<template_api>(
			internal::load_or_create_config<template_config>(),
			std::make_unique<http_service>(),
			nullptr, //create_exchange_websocket_stream<internal::coinbase_websocket_stream>(),
			testing);
	}
}