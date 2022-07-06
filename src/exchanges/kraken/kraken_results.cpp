#include "kraken_results.h"
#include "common/json/json.h"
#include "common/utils/stringutils.h"
#include "common/utils/timeutils.h"
#include "logging/logger.h"

namespace
{
	using namespace mb;

	std::string get_error(json_document& json)
	{
		std::vector<std::string> messages = json.get<std::vector<std::string>>("error");
		
		return messages.empty()
			? ""
			: messages[0];
	}

	template<typename T, typename Reader> 
	result<T> read_result(std::string_view jsonResult, const Reader& reader)
	{
		try
		{
			json_document jsonDocument{ parse_json(jsonResult) };

			std::string error = get_error(jsonDocument);
			if (!error.empty())
			{
				return result<T>::fail(std::move(error));
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

	result<std::vector<order_description>> read_open_closed_orders(std::string_view jsonResult, std::string_view orderStatus)
	{
		return read_result<std::vector<order_description>>(jsonResult, [&orderStatus](const json_element& resultElement)
			{
				json_element ordersElement{ resultElement.element(orderStatus) };
				std::vector<order_description> orderDescriptions;
				orderDescriptions.reserve(ordersElement.size());

				for (auto it = ordersElement.begin(); it != ordersElement.end(); ++it)
				{
					std::string orderId{ it.key() };
					json_element order{ it.value() };
					
					std::string status{ order.get<std::string>("status") };
					if (status != orderStatus)
					{
						continue;
					}

					json_element descriptionElement{ order.element("descr") };

					std::string pairName{ descriptionElement.get<std::string>("pair") };
					trade_action action = descriptionElement.get<std::string>("type") == "buy" 
						? trade_action::BUY 
						: trade_action::SELL;
					double price = std::stod(descriptionElement.get<std::string>("price"));
					double volume = std::stod(order.get<std::string>("vol"));

					orderDescriptions.emplace_back(std::move(orderId), std::move(pairName), action, price, volume);
				}

				return orderDescriptions;
			});
	}
}

namespace mb::kraken
{
	result<exchange_status> read_system_status(std::string_view jsonResult)
	{
		return read_result<exchange_status>(jsonResult, [](const json_element& resultElement)
		{
			std::string status_string{ resultElement.get<std::string>("status") };

			if (status_string == "online")
			{
				return exchange_status::ONLINE;
			}
			else if (status_string == "cancel_only")
			{
				return exchange_status::CANCEL_ONLY;
			}
			else if (status_string == "post_only")
			{
				return exchange_status::POST_ONLY;
			}
			else
			{
				return exchange_status::MAINTENANCE;
			}
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
				std::vector<std::string> assetSymbols{ split(it.value().get<std::string>("wsname"), '/') };
				pairs.emplace_back(std::string{ assetSymbols[0] }, std::string{ assetSymbols[1] });
			}

			return pairs;
		});
	}

	result<std::vector<ohlcv_data>> read_ohlcv_data(std::string_view jsonResult, int count)
	{
		return read_result<std::vector<ohlcv_data>>(jsonResult, [count](const json_element& resultElement)
		{
			json_element pairElement{ resultElement.begin().value() };
			int limit{ std::min<int>(pairElement.size(), count) };
			std::vector<ohlcv_data> data;
			data.reserve(limit);

			for (int i = 0; i < limit; ++i)
			{
				json_element dataElement{ pairElement.element(pairElement.size() - i - 1) };

				data.emplace_back(
					dataElement.get<std::time_t>(0),
					std::stod(dataElement.get<std::string>(1)),
					std::stod(dataElement.get<std::string>(2)),
					std::stod(dataElement.get<std::string>(3)),
					std::stod(dataElement.get<std::string>(4)),
					std::stod(dataElement.get<std::string>(6)));
			}

			return data;
		});
	}

	result<double> read_price(std::string_view jsonResult)
	{
		return read_result<double>(jsonResult, [](const json_element& resultElement)
		{
			json_element dataElement{ resultElement.begin().value() };
			return std::stod(dataElement.get<std::vector<std::string>>("c")[0]);
		});
	}

	result<order_book_state> read_order_book(std::string_view jsonResult)
	{
		return read_result<order_book_state>(jsonResult, [](const json_element& resultElement)
		{
			json_element bookElement{ resultElement.begin().value() };

			json_element asks{ bookElement.element("asks") };
			json_element bids{ bookElement.element("bids") };

			int depth = asks.size();

			std::vector<order_book_entry> askEntries;
			askEntries.reserve(depth);

			std::vector<order_book_entry> bidEntries;
			bidEntries.reserve(depth);

			for (int i = 0; i < depth; i++)
			{
				json_element asks_i{ asks.element(i) };
				askEntries.emplace_back(
					std::stod(asks_i.element(0).get<std::string>()),
					std::stod(asks_i.element(1).get<std::string>()),
					order_book_side::ASK);

				json_element bids_i{ bids.element(i) };
				bidEntries.emplace_back(
					std::stod(bids_i.element(0).get<std::string>()),
					std::stod(bids_i.element(1).get<std::string>()),
					order_book_side::BID);
			}

			return order_book_state{ std::move(askEntries), std::move(bidEntries) };
		});
	}

	result<unordered_string_map<double>> read_balances(std::string_view jsonResult)
	{
		return read_result<unordered_string_map<double>>(jsonResult, [](const json_element& resultElement)
		{
			unordered_string_map<double> balances;
			balances.reserve(resultElement.size());

			for (auto it = resultElement.begin(); it != resultElement.end(); ++it)
			{
				balances.emplace(it.key(), std::stod(it.value().get<std::string>()));
			}

			return balances;
		});
	}

	result<double> read_fee(std::string_view jsonResult)
	{
		return read_result<double>(jsonResult, [](const json_element& resultElement)
		{
			json_element feeElement{ resultElement.element("fees").begin().value() };
			std::string fee{ feeElement.get<std::string>("fee") };

			return std::stod(fee);
		});
	}

	result<std::vector<order_description>> read_open_orders(std::string_view jsonResult)
	{
		return read_open_closed_orders(jsonResult, "open");
	}

	result<std::vector<order_description>> read_closed_orders(std::string_view jsonResult)
	{
		return read_open_closed_orders(jsonResult, "closed");
	}

	result<std::string> read_add_order(std::string_view jsonResult)
	{
		return read_result<std::string>(jsonResult, [](const json_element& resultElement)
		{
			return resultElement.get<std::vector<std::string>>("txid")[0];
		});
	}

	result<void> read_cancel_order(std::string_view jsonResult)
	{
		return read_result<void>(jsonResult, [](const json_element& resultElement)
		{
		});
	}
}