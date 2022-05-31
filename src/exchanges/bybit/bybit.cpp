#include "bybit.h"
#include "bybit_results.h"
#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	bybit_api::bybit_api(
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream)
		: 
		_httpService{ std::move(httpService) },
		_websocketStream{ websocketStream }
	{}

	exchange_status bybit_api::get_status() const
	{
		return mb::bybit::read_system_status("").value();
	}

	std::vector<tradable_pair> bybit_api::get_tradable_pairs() const
	{
		return mb::bybit::read_tradable_pairs("").value();
	}

	ohlcv_data bybit_api::get_24h_stats(const tradable_pair& tradablePair) const
	{
		return mb::bybit::read_24h_stats("").value();
	}

	double bybit_api::get_price(const tradable_pair& tradablePair) const
	{
		return mb::bybit::read_price("").value();
	}

	order_book_state bybit_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		return mb::bybit::read_order_book("").value();
	}

	double bybit_api::get_fee(const tradable_pair& tradablePair) const
	{
		return mb::bybit::read_fee("").value();
	}

	unordered_string_map<double> bybit_api::get_balances() const
	{
		return mb::bybit::read_balances("").value();
	}

	std::vector<order_description> bybit_api::get_open_orders() const
	{
		return mb::bybit::read_open_orders("").value();
	}

	std::vector<order_description> bybit_api::get_closed_orders() const
	{
		return mb::bybit::read_closed_orders("").value();
	}

	std::string bybit_api::add_order(const trade_description& description)
	{
		return mb::bybit::read_add_order("").value();
	}

	void bybit_api::cancel_order(std::string_view orderId)
	{
		mb::bybit::read_cancel_order("").value();
	}

	std::unique_ptr<exchange> make_bybit(std::shared_ptr<websocket_client> websocketClient)
	{
		throw not_implemented_exception{ "TEMPLATE" };
	}
}