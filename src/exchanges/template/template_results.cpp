#include "template_results.h"

namespace mb::read_template
{
	result<exchange_status> read_system_status(std::string_view jsonResult)
	{
		return result<exchange_status>::success(exchange_status::ONLINE);
	}

	result<std::vector<tradable_pair>> read_tradable_pairs(std::string_view jsonResult)
	{
		return result<std::vector<tradable_pair>>::success({});
	}

	result<std::vector<ohlcv_data>> read_ohlcv(std::string_view jsonResult)
	{
		return result<std::vector<ohlcv_data>>::success({});
	}

	result<double> read_price(std::string_view jsonResult)
	{
		return result<double>::success(0.0);
	}

	result<order_book_state> read_order_book(std::string_view jsonResult)
	{
		return result<order_book_state>::success(order_book_state{ 0, {},{} });
	}

	result<std::unordered_map<std::string,double>> read_balances(std::string_view jsonResult)
	{
		return result<std::unordered_map<std::string,double>>::success({});
	}

	result<double> read_fee(std::string_view jsonResult)
	{
		return result<double>::success(0.0);
	}

	result<std::vector<order_description>> read_open_orders(std::string_view jsonResult)
	{
		return result<std::vector<order_description>>::success({});
	}

	result<std::vector<order_description>> read_closed_orders(std::string_view jsonResult)
	{
		return result<std::vector<order_description>>::success({});
	}

	result<std::string> read_add_order(std::string_view jsonResult)
	{
		return result<std::string>::success("");
	}

	result<void> read_cancel_order(std::string_view jsonResult)
	{
		return result<void>::success();
	}
}