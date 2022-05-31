#include "digifinex.h"
#include "digifinex_results.h"
#include "common/exceptions/not_implemented_exception.h"

namespace mb
{
	digifinex_api::digifinex_api(
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream)
		: 
		_httpService{ std::move(httpService) },
		_websocketStream{ websocketStream }
	{}

	exchange_status digifinex_api::get_status() const
	{
		return mb::digifinex::read_system_status("").value();
	}

	std::vector<tradable_pair> digifinex_api::get_tradable_pairs() const
	{
		return mb::digifinex::read_tradable_pairs("").value();
	}

	ohlcv_data digifinex_api::get_24h_stats(const tradable_pair& tradablePair) const
	{
		return mb::digifinex::read_24h_stats("").value();
	}

	double digifinex_api::get_price(const tradable_pair& tradablePair) const
	{
		return mb::digifinex::read_price("").value();
	}

	order_book_state digifinex_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		return mb::digifinex::read_order_book("").value();
	}

	double digifinex_api::get_fee(const tradable_pair& tradablePair) const
	{
		return mb::digifinex::read_fee("").value();
	}

	unordered_string_map<double> digifinex_api::get_balances() const
	{
		return mb::digifinex::read_balances("").value();
	}

	std::vector<order_description> digifinex_api::get_open_orders() const
	{
		return mb::digifinex::read_open_orders("").value();
	}

	std::vector<order_description> digifinex_api::get_closed_orders() const
	{
		return mb::digifinex::read_closed_orders("").value();
	}

	std::string digifinex_api::add_order(const trade_description& description)
	{
		return mb::digifinex::read_add_order("").value();
	}

	void digifinex_api::cancel_order(std::string_view orderId)
	{
		mb::digifinex::read_cancel_order("").value();
	}

	std::unique_ptr<exchange> make_digifinex(std::shared_ptr<websocket_client> websocketClient)
	{
		throw not_implemented_exception{ "TEMPLATE" };
	}
}