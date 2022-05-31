#include <unordered_set>

#include "bybit_results.h"
#include "common/json/json.h"

namespace
{
	using namespace mb;

	template<typename T, typename Reader>
	result<T> read_result(std::string_view jsonResult, const Reader& reader)
	{
		try
		{
			json_document jsonDocument{ parse_json(jsonResult) };

			if (jsonDocument.get<int>("ret_code") != 0)
			{
				return result<T>::fail(jsonDocument.get<std::string>("ret_msg"));
			}

			return result<T>::success(reader(jsonDocument.element("result")));
		}
		catch (const std::exception& e)
		{
			return result<T>::fail(e.what());
		}
	}
}

namespace mb::bybit
{
	result<exchange_status> read_system_status(std::string_view jsonResult)
	{
		return result<exchange_status>::success(exchange_status::ONLINE);
	}

	result<std::vector<tradable_pair>> read_tradable_pairs(std::string_view jsonResult)
	{
		return read_result<std::vector<tradable_pair>>(jsonResult, [](const json_element& resultElement)
		{
			std::unordered_set<tradable_pair> pairs;
			pairs.reserve(resultElement.size());

			for (auto it = resultElement.begin(); it != resultElement.end(); ++it)
			{
				json_element pairElement{ it.value() };

				std::string asset{ pairElement.get<std::string>("base_currency") };
				std::string priceUnit{ pairElement.get<std::string>("quote_currency") };

				pairs.emplace(std::move(asset), std::move(priceUnit));
			}

			return std::vector<tradable_pair>{ pairs.begin(), pairs.end() };
		});
	}

	result<ohlcv_data> read_24h_stats(std::string_view jsonResult)
	{
		return result<ohlcv_data>::success(ohlcv_data{ 0.0,0.0,0.0,0.0,0.0 });
	}

	result<double> read_price(std::string_view jsonResult)
	{
		return result<double>::success(0.0);
	}

	result<order_book_state> read_order_book(std::string_view jsonResult)
	{
		return result<order_book_state>::success(order_book_state{ {},{} });
	}

	result<unordered_string_map<double>> read_balances(std::string_view jsonResult)
	{
		return read_result<unordered_string_map<double>>(jsonResult, [](const json_element& resultElement)
		{
			unordered_string_map<double> balances;
			balances.reserve(resultElement.size());

			for (auto it = resultElement.begin(); it != resultElement.end(); ++it)
			{
				json_element pairElement{ it.value() };

				std::string asset{ it.key() };
				double balance{ pairElement.get<double>("available_balance") };

				balances.emplace(std::move(asset), std::move(balance));
			}

			return balances;
		});
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