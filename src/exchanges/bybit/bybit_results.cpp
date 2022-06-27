#include <unordered_set>

#include "bybit_results.h"
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

			if (jsonDocument.get<int>("ret_code") != 0)
			{
				return result<T>::fail(jsonDocument.get<std::string>("ret_msg"));
			}

			if constexpr (std::is_same_v<T, void>)
			{
				return result<T>::success();
			}
			else
			{
				return result<T>::success(reader(jsonDocument.element("result")));
			}
		}
		catch (const std::exception& e)
		{
			return result<T>::fail(e.what());
		}
	}

	trade_action to_trade_action(std::string_view sideString)
	{
		return sideString == "BUY"
			? trade_action::BUY : trade_action::SELL;
	}

	order_book_entry read_order_book_entry(const json_element& entryElement, order_book_side side)
	{
		return order_book_entry
		{
			std::stod(entryElement.element(0).get<std::string>()),
			std::stod(entryElement.element(1).get<std::string>()),
			side
		};
	}

	order_description read_order_description(const json_element& orderElement)
	{
		trade_action action = to_trade_action(orderElement.get<std::string>("side"));
		double price = std::stod(orderElement.get<std::string>("price"));
		double qty = std::stod(orderElement.get<std::string>("origQty"));

		if (action == trade_action::BUY && orderElement.get<std::string>("type") == "MARKET")
		{
			qty = calculate_volume(price, qty);
		}
		
		return order_description
		{
			orderElement.get<std::string>("orderId"),
			orderElement.get<std::string>("symbol"),
			action,
			price,
			qty
		};
	}
}

namespace mb::bybit
{
	result<exchange_status> read_system_status(std::string_view jsonResult)
	{
		return read_result<exchange_status>(jsonResult, [](const json_element& resultElement)
		{
			return exchange_status::ONLINE;
		});
	}

	result<std::vector<tradable_pair>> read_tradable_pairs(std::string_view jsonResult)
	{
		return read_result<std::vector<tradable_pair>>(jsonResult, [](const json_element& resultElement)
		{
			std::vector<tradable_pair> pairs;
			pairs.reserve(resultElement.size());

			for (auto it = resultElement.begin(); it != resultElement.end(); ++it)
			{
				json_element pairElement{ it.value() };

				std::string asset{ pairElement.get<std::string>("baseCurrency") };
				std::string priceUnit{ pairElement.get<std::string>("quoteCurrency") };

				pairs.emplace_back(std::move(asset), std::move(priceUnit));
			}

			return pairs;
		});
	}

	result<ohlcv_data> read_24h_stats(std::string_view jsonResult)
	{
		return read_result<ohlcv_data>(jsonResult, [](const json_element& resultElement)
		{
			return ohlcv_data
			{
				std::stod(resultElement.get<std::string>("openPrice")),
				std::stod(resultElement.get<std::string>("highPrice")),
				std::stod(resultElement.get<std::string>("lowPrice")),
				std::stod(resultElement.get<std::string>("lastPrice")),
				std::stod(resultElement.get<std::string>("volume"))
			};
		});
	}

	result<std::vector<timed_ohlcv_data>> read_ohlcv(std::string_view jsonResult)
	{
		return read_result<std::vector<timed_ohlcv_data>>(jsonResult, [](const json_element& resultElement)
		{
			std::vector<timed_ohlcv_data> ohlcvData;
			ohlcvData.reserve(resultElement.size());

			for (auto it = resultElement.begin(); it != resultElement.end(); ++it)
			{
				json_element ohlcvElement = it.value();

				ohlcvData.emplace_back(
					ohlcvElement.get<std::time_t>(0),
					ohlcv_data
					{
						std::stod(ohlcvElement.get<std::string>(1)),
						std::stod(ohlcvElement.get<std::string>(2)),
						std::stod(ohlcvElement.get<std::string>(3)),
						std::stod(ohlcvElement.get<std::string>(4)),
						std::stod(ohlcvElement.get<std::string>(5))
					});
			}

			return ohlcvData;
		});
	}

	result<double> read_price(std::string_view jsonResult)
	{
		return read_result<double>(jsonResult, [](const json_element& resultElement)
		{
			return std::stod(resultElement.get<std::string>("price"));
		});
	}

	result<order_book_state> read_order_book(std::string_view jsonResult)
	{
		return read_result<order_book_state>(jsonResult, [](const json_element& resultElement)
		{
			json_element asksElement{ resultElement.element("asks") };
			json_element bidsElement{ resultElement.element("bids") };

			std::vector<order_book_entry> askEntries;
			askEntries.reserve(asksElement.size());

			std::vector<order_book_entry> bidEntries;
			bidEntries.reserve(bidsElement.size());

			int depth = std::max(asksElement.size(), bidsElement.size());

			auto asksIt = asksElement.begin();
			auto bidsIt = bidsElement.begin();

			for (int i = 0; i < depth; ++i)
			{
				if (asksIt != asksElement.end())
				{
					json_element entryElement{ asksIt.value() };
					askEntries.emplace_back(read_order_book_entry(entryElement, order_book_side::ASK));

					++asksIt;
				}
				if (bidsIt != bidsElement.end())
				{
					json_element entryElement{ bidsIt.value() };
					bidEntries.emplace_back(read_order_book_entry(entryElement, order_book_side::BID));

					++bidsIt;
				}
			}

			return order_book_state{ std::move(askEntries), std::move(bidEntries) };
		});
	}

	result<unordered_string_map<double>> read_balances(std::string_view jsonResult)
	{
		return read_result<unordered_string_map<double>>(jsonResult, [](const json_element& resultElement)
		{
			json_element balancesElement{ resultElement.element("balances") };

			unordered_string_map<double> balances;
			balances.reserve(balancesElement.size());

			for (auto it = balancesElement.begin(); it != balancesElement.end(); ++it)
			{
				json_element assetElement{ it.value() };

				std::string asset{ assetElement.get<std::string>("coin") };
				double balance{ std::stod(assetElement.get<std::string>("free")) };

				balances.emplace(std::move(asset), std::move(balance));
			}

			return balances;
		});
	}

	result<std::vector<order_description>> read_open_orders(std::string_view jsonResult)
	{
		return read_result<std::vector<order_description>>(jsonResult, [](const json_element& resultElement)
		{
			std::vector<order_description> orders;
			orders.reserve(resultElement.size());

			for (auto it = resultElement.begin(); it != resultElement.end(); ++it)
			{
				json_element orderElement{ it.value() };

				orders.emplace_back(read_order_description(orderElement));
			}

			return orders;
		});
	}

	result<std::vector<order_description>> read_closed_orders(std::string_view jsonResult)
	{
		return read_result<std::vector<order_description>>(jsonResult, [](const json_element& resultElement)
		{
			std::vector<order_description> orders;
			orders.reserve(resultElement.size());

			for (auto it = resultElement.begin(); it != resultElement.end(); ++it)
			{
				json_element orderElement{ it.value() };

				if (orderElement.get<std::string>("status") == "FILLED")
				{
					orders.emplace_back(read_order_description(orderElement));
				}
			}

			return orders;
		});
	}

	result<std::string> read_add_order(std::string_view jsonResult)
	{
		return read_result<std::string>(jsonResult, [](const json_element& resultElement)
		{
			return resultElement.get<std::string>("orderId");
		});
	}

	result<void> read_cancel_order(std::string_view jsonResult)
	{
		return read_result<void>(jsonResult, [](const json_element& resultElement)
		{
		});
	}
}