#include "coinbase_results.h"

namespace cb::coinbase
{
	result<std::vector<tradable_pair>> read_tradable_pairs(std::string_view jsonResult)
	{
		return result<std::vector<tradable_pair>>::success(std::vector<tradable_pair>{});
	}

	result<pair_stats> read_24h_stats(std::string_view jsonResult)
	{
		return result<pair_stats>::success(pair_stats{ 0, 0, 0, 0 });
	}

	result<order_book_state> read_order_book(std::string_view jsonResult, int depth)
	{
		return result<order_book_state>::success(std::vector<order_book_level>{});
	}

	result<double> read_fee(std::string_view jsonResult)
	{
		return result<double>::success(0.0);
	}

	result<std::unordered_map<asset_symbol, double>> read_balances(std::string_view jsonResult)
	{
		return result<std::unordered_map<asset_symbol, double>>::success(std::unordered_map<asset_symbol, double>{});
	}

	result<std::vector<order_description>> read_orders(std::string_view jsonResult)
	{
		return result<std::vector<order_description>>::success(std::vector<order_description>{});
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