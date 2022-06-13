//#include "template.h"
//#include "template_results.h"
//#include "common/exceptions/not_implemented_exception.h"
//
//namespace mb
//{
//	template_api::template_api(
//		std::unique_ptr<http_service> httpService,
//		std::shared_ptr<websocket_stream> websocketStream)
//		: 
//		_httpService{ std::move(httpService) },
//		_websocketStream{ websocketStream }
//	{}
//
//	exchange_status template_api::get_status() const
//	{
//		return mb::read_template::read_system_status("").value();
//	}
//
//	std::vector<tradable_pair> template_api::get_tradable_pairs() const
//	{
//		return mb::read_template::read_tradable_pairs("").value();
//	}
//
//	ohlcv_data template_api::get_24h_stats(const tradable_pair& tradablePair) const
//	{
//		return mb::read_template::read_24h_stats("").value();
//	}
//
//	double template_api::get_price(const tradable_pair& tradablePair) const
//	{
//		return mb::read_template::read_price("").value();
//	}
//
//	order_book_state template_api::get_order_book(const tradable_pair& tradablePair, int depth) const
//	{
//		return mb::read_template::read_order_book("").value();
//	}
//
//	double template_api::get_fee(const tradable_pair& tradablePair) const
//	{
//		return mb::read_template::read_fee("").value();
//	}
//
//	unordered_string_map<double> template_api::get_balances() const
//	{
//		return mb::read_template::read_balances("").value();
//	}
//
//	std::vector<order_description> template_api::get_open_orders() const
//	{
//		return mb::read_template::read_open_orders("").value();
//	}
//
//	std::vector<order_description> template_api::get_closed_orders() const
//	{
//		return mb::read_template::read_closed_orders("").value();
//	}
//
//	std::string template_api::add_order(const trade_description& description)
//	{
//		return mb::read_template::read_add_order("").value();
//	}
//
//	void template_api::cancel_order(std::string_view orderId)
//	{
//		mb::read_template::read_cancel_order("").value();
//	}
//
//	std::unique_ptr<exchange> make_template(std::shared_ptr<websocket_client> websocketClient)
//	{
//		throw not_implemented_exception{"TEMPLATE"};
//	}
//}