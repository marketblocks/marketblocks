#include <unordered_set>

#include "dextrade_results.h"
#include "common/json/json.h"
#include "common/exceptions/not_implemented_exception.h"

namespace
{
	using namespace mb;

	template<typename T, typename Reader>
	result<T> read_result(std::string_view jsonResult, const Reader& reader)
	{
		try
		{
			json_document jsonDocument{ parse_json(jsonResult) };

			bool success{ jsonDocument.get<bool>("status") };
			if (!success)
			{
				std::string error{ jsonDocument.get<std::string>("error") };
				return result<T>::fail(std::move(error));
			}

			return result<T>::success(reader(jsonDocument.element("data")));
		}
		catch (const std::exception& e)
		{
			return result<T>::fail(e.what());
		}
	}
}

namespace mb::dextrade
{
	result<std::vector<tradable_pair>> read_tradable_pairs(std::string_view jsonResult)
	{
		return read_result<std::vector<tradable_pair>>(jsonResult, [](const json_element& dataElement)
		{
			std::vector<tradable_pair> pairs;
			pairs.reserve(dataElement.size());

			for (auto it = dataElement.begin(); it != dataElement.end(); ++it)
			{
				json_element pairElement{ it.value() };

				std::string asset{ pairElement.get<std::string>("base") };
				std::string priceUnit{ pairElement.get<std::string>("quote") };

				pairs.emplace_back(std::move(asset), std::move(priceUnit));
			}

			return std::vector<tradable_pair>{ pairs.begin(), pairs.end() };
		});
	}

	result<ohlcv_data> read_24h_stats(std::string_view jsonResult)
	{
		throw not_implemented_exception{ "digifinex::read_24h_stats" };
	}

	result<double> read_price(std::string_view jsonResult)
	{
		return read_result<double>(jsonResult, [](const json_element& dataElement)
		{
			return std::stod(dataElement.get<std::string>("last"));
		});
	}

	result<order_book_state> read_order_book(std::string_view jsonResult)
	{
		return read_result<order_book_state>(jsonResult, [](const json_element& dataElement)
		{
			json_element bidsElement{ dataElement.element("buy") };
			json_element asksElement{ dataElement.element("sell") };

			std::vector<order_book_entry> bids;
			bids.reserve(bidsElement.size());

			std::vector<order_book_entry> asks;
			asks.reserve(asksElement.size());

			int depth = std::max(bidsElement.size(), asksElement.size());

			json_iterator bidsIt{ bidsElement.begin() };
			json_iterator asksIt{ asksElement.begin() };

			for (int i = 0; i < depth; ++i)
			{
				if (asksIt != asksElement.end())
				{
					json_element entryElement{ asksIt.value() };
					asks.emplace_back(
						entryElement.get<double>("rate"),
						entryElement.get<double>("volume"),
						order_book_side::ASK);

					++asksIt;
				}

				if (bidsIt != bidsElement.end())
				{
					json_element entryElement{ bidsIt.value() };
					bids.emplace_back(
						entryElement.get<double>("rate"),
						entryElement.get<double>("volume"),
						order_book_side::BID);

					++bidsIt;
				}
			}

			return order_book_state{ std::move(asks), std::move(bids) };
		});
	}

	result<std::unordered_map<std::string,double>> read_balances(std::string_view jsonResult)
	{
		return read_result<std::unordered_map<std::string,double>>(jsonResult, [](const json_element& dataElement)
		{
			std::unordered_map<std::string,double> balances;
			/*balances.reserve(resultElement.size());

			for (auto it = resultElement.begin(); it != resultElement.end(); ++it)
			{
				json_element balanceElement{ it.value() };

				std::string asset{ balanceElement.get<std::string>("currency") };
				double balance{ balanceElement.get<double>("free") };

				balances.emplace(std::move(asset), std::move(balance));
			}*/

			return balances;
		});
	}

	result<std::vector<order_description>> read_open_orders(std::string_view jsonResult)
	{
		throw not_implemented_exception{ "digifinex::read_open_orders" };
	}

	result<std::vector<order_description>> read_closed_orders(std::string_view jsonResult)
	{
		throw not_implemented_exception{ "digifinex::read_closed_orders" };
	}

	result<std::string> read_add_order(std::string_view jsonResult)
	{
		return read_result<std::string>(jsonResult, [](const json_element& dataElement)
		{
			return dataElement.get<std::string>("id");
		});
	}

	result<void> read_cancel_order(std::string_view jsonResult)
	{
		throw not_implemented_exception{ "digifinex::read_cancel_order" };
	}
}