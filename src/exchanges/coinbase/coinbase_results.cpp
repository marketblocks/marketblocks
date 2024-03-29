#include <chrono>

#include "coinbase_results.h"
#include "common/json/json.h"
#include "common/utils/timeutils.h"
#include "common/utils/financeutils.h"

namespace
{
	using namespace mb;

	template<typename T, typename Reader>
	result<T> read_result(std::string_view jsonResult, const Reader& reader)
	{
		try
		{
			json_document jsonDocument{ parse_json(jsonResult) };

			if (jsonDocument.has_member("message"))
			{
				return result<T>::fail(jsonDocument.get<std::string>("message"));
			}

			if constexpr (std::is_same_v<T, void>)
			{
				return result<T>::success();
			}
			else
			{
				return result<T>::success(reader(jsonDocument));
			}
		}
		catch (const std::exception& e)
		{
			return result<T>::fail(e.what());
		}
	}

	trade_action to_trade_action(std::string_view actionString)
	{
		if (actionString == "buy")
		{
			return trade_action::BUY;
		}

		return trade_action::SELL;
	}

	order_type to_order_type(std::string_view orderTypeString)
	{
		if (orderTypeString == "limit")
		{
			return order_type::LIMIT;
		}
		if (orderTypeString == "market")
		{
			return order_type::MARKET;
		}
		if (orderTypeString == "stop")
		{
			return order_type::STOP_LOSS;
		}
	}
}

namespace mb::coinbase
{
	result<std::vector<tradable_pair>> read_tradable_pairs(std::string_view jsonResult)
	{
		return read_result<std::vector<tradable_pair>>(jsonResult, [](const json_document& json)
		{
			std::vector<tradable_pair> pairs;
			pairs.reserve(json.size());

			for (auto it = json.begin(); it != json.end(); ++it)
			{
				json_element pairElement{ it.value() };

				std::string asset{ pairElement.get<std::string>("base_currency")};
				std::string priceUnit{ pairElement.get<std::string>("quote_currency")};

				pairs.emplace_back(std::move(asset), std::move(priceUnit));
			}

			return pairs;
		});
	}

	result<std::vector<ohlcv_data>> read_ohlcv_data(std::string_view jsonResult, int count)
	{
		return read_result<std::vector<ohlcv_data>>(jsonResult, [count](const json_document& json)
		{
			int limit{ std::min<int>(json.size(), count) };
			std::vector<ohlcv_data> data;
			data.reserve(limit);

			for (int i = 0; i < limit; ++i)
			{
				json_element dataElement{ json.element(i) };
				data.emplace_back(
					dataElement.get<std::time_t>(0),
					dataElement.get<double>(3),
					dataElement.get<double>(2),
					dataElement.get<double>(1),
					dataElement.get<double>(4),
					dataElement.get<double>(5));
			}

			return data;
		});
	}

	result<double> read_price(std::string_view jsonResult)
	{
		return read_result<double>(jsonResult, [](const json_document& json)
		{
			return std::stod(json.get<std::string>("price"));
		});
	}

	result<order_book_state> read_order_book(std::string_view jsonResult, int depth)
	{
		static constexpr int PRICE_INDEX = 0;
		static constexpr int VOLUME_INDEX = 1;

		return read_result<order_book_state>(jsonResult, [depth](const json_document& json)
		{
			json_element bids{ json.element("bids") };
			json_element asks{ json.element("asks") };

			std::vector<order_book_entry> bidEntries;
			bidEntries.reserve(depth);

			std::vector<order_book_entry> askEntries;
			askEntries.reserve(depth);

			for (int i = 0; i < depth; ++i)
			{
				if (i < bids.size())
				{
					json_element bidElement{ bids.element(i) };

					bidEntries.emplace_back(
						std::stod(bidElement.get<std::string>(PRICE_INDEX)),
						std::stod(bidElement.get<std::string>(VOLUME_INDEX)),
						order_book_side::ASK);
				}

				if (i < asks.size())
				{
					json_element askElement{ asks.element(i) };

					askEntries.emplace_back(
						std::stod(askElement.get<std::string>(PRICE_INDEX)),
						std::stod(askElement.get<std::string>(VOLUME_INDEX)),
						order_book_side::BID);
				}
			}

			return order_book_state{ json.get<std::time_t>("sequence"), std::move(askEntries), std::move(bidEntries)};
		});
	}

	result<double> read_fee(std::string_view jsonResult)
	{
		return read_result<double>(jsonResult, [](const json_document& json)
		{
			return std::stod(json.get<std::string>("taker_fee_rate")) * 100;
		});
	}

	result<std::unordered_map<std::string,double>> read_balances(std::string_view jsonResult)
	{
		return read_result<std::unordered_map<std::string,double>>(jsonResult, [](const json_document& json)
		{
			std::unordered_map<std::string,double> balances;
			balances.reserve(json.size());

			for (auto it = json.begin(); it != json.end(); ++it)
			{
				json_element balanceElement{ it.value() };

				balances.emplace(
					balanceElement.get<std::string>("currency"),
					std::stod(balanceElement.get<std::string>("balance")));
			}

			return balances;
		});
	}

	result<std::vector<order_description>> read_orders(std::string_view jsonResult)
	{
		return read_result<std::vector<order_description>>(jsonResult, [](const json_document& json)
		{
			std::vector<order_description> orders;
			orders.reserve(json.size());

			for (auto it = json.begin(); it != json.end(); ++it)
			{
				json_element orderElement{ it.value() };

				double size = std::stod(orderElement.get<std::string>("size"));
				double price;
				if (orderElement.has_member("price"))
				{
					price = std::stod(orderElement.get<std::string>("price"));
				}
				else
				{
					double cost = orderElement.has_member("funds")
						? std::stod(orderElement.get<std::string>("funds"))
						: std::stod(orderElement.get<std::string>("executed_value"));

					price = calculate_asset_price(cost, size);
				}

				std::string createdAt{ orderElement.get<std::string>("created_at") };
				std::time_t time{ to_time_t(createdAt, "%Y-%m-%dT%T") };

				orders.emplace_back(
					time,
					orderElement.get<std::string>("id"),
					to_order_type(orderElement.get<std::string>("type")),
					orderElement.get<std::string>("product_id"),
					to_trade_action(orderElement.get<std::string>("side")),
					price,
					size);
			}

			return orders;
		});
	}

	result<std::string> read_add_order(std::string_view jsonResult)
	{
		return read_result<std::string>(jsonResult, [](const json_document& json)
		{
			return json.get<std::string>("id");
		});
	}

	result<void> read_cancel_order(std::string_view jsonResult)
	{
		return read_result<void>(jsonResult, [](const json_document& json)
			{
			});
	}
}