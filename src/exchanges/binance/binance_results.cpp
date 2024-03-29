#include "binance_results.h"
#include "common/json/json.h"
#include "common/utils/stringutils.h"

#include "common/exceptions/not_implemented_exception.h"

namespace
{
	using namespace mb;

	order_book_entry read_order_book_entry(json_iterator& iterator, order_book_side side)
	{
		json_element element{ iterator.value() };

		return order_book_entry
		{
			std::stod(element.get<std::string>(0)),
			std::stod(element.get<std::string>(1)),
			side
		};
	}

	trade_action to_trade_action(std::string_view action)
	{
		if (action == "BUY")
			return trade_action::BUY;

		return trade_action::SELL;
	}

	order_type to_order_type(std::string_view orderTypeString)
	{
		if (orderTypeString == "LIMIT")
		{
			return order_type::LIMIT;
		}
		if (orderTypeString == "MARKET")
		{
			return order_type::MARKET;
		}
		if (mb::contains(orderTypeString.data(), "STOP_LOSS"))
		{
			return order_type::STOP_LOSS;
		}
		if (mb::contains(orderTypeString.data(), "TAKE_PROFIT"))
		{
			return order_type::TAKE_PROFIT;
		}

		throw std::invalid_argument{ "Unknown order type" };
	}

	template<typename Json>
	order_description read_order_description(const Json& orderElement)
	{
		return order_description
		{
			orderElement.template get<std::time_t>("time") / 1000,
			std::to_string(orderElement.template get<long long>("orderId")),
			to_order_type(orderElement.template get<std::string>("type")),
			orderElement.template get<std::string>("symbol"),
			to_trade_action(orderElement.template get<std::string>("side")),
			std::stod(orderElement.template get<std::string>("price")),
			std::stod(orderElement.template get<std::string>("origQty"))
		};
	}

	template<typename T, typename Reader>
	result<T> read_result(std::string_view jsonResult, const Reader& reader)
	{
		try
		{
			json_document jsonDocument{ parse_json(jsonResult) };

			if (jsonDocument.has_member("code"))
			{
				return result<T>::fail(jsonDocument.get<std::string>("msg"));
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

	order_status to_order_status(std::string_view statusString)
	{
		if (statusString == "NEW")
		{
			return order_status::OPEN;
		}
		if (statusString == "PARTIALLY_FILLED")
		{
			return order_status::PARTIALLY_FILLED;
		}
		if (statusString == "FILLED")
		{
			return order_status::CLOSED;
		}

		return order_status::CANCELLED;
	}
}

namespace mb::binance
{
	result<exchange_status> read_system_status(std::string_view jsonResult)
	{
		return read_result<exchange_status>(jsonResult, [](const json_document& json)
		{
			return exchange_status::ONLINE;
		});
	}

	result<std::unordered_map<tradable_pair, internal::binance_order_filters>> read_tradable_pairs(std::string_view jsonResult)
	{
		return read_result<std::unordered_map<tradable_pair, internal::binance_order_filters>>(jsonResult, [](const json_document& json)
		{
			json_element symbolsElement{ json.element("symbols") };
			std::unordered_map<tradable_pair, internal::binance_order_filters> pairs;
			pairs.reserve(symbolsElement.size());

			for (auto it = symbolsElement.begin(); it != symbolsElement.end(); ++it)
			{
				json_element pairElement{ it.value() };

				std::string asset{ pairElement.get<std::string>("baseAsset") };
				std::string priceUnit{ pairElement.get<std::string>("quoteAsset") };
				tradable_pair pair{ std::move(asset), std::move(priceUnit) };

				double tickSize = 0.0;
				double minQty = 0.0;
				double qtyStepSize = 0.0;
				double minValue = 0.0;
				json_element filtersElement{ pairElement.element("filters") };

				for (auto filterIt = filtersElement.begin(); filterIt != filtersElement.end(); ++filterIt)
				{
					json_element filter{ filterIt.value() };
					std::string filterType{ filter.get<std::string>("filterType") };

					if (filterType == "PRICE_FILTER")
					{
						tickSize = std::stod(filter.get<std::string>("tickSize"));
					}
					else if (filterType == "LOT_SIZE")
					{
						minQty = std::stod(filter.get<std::string>("minQty"));
						qtyStepSize = std::stod(filter.get<std::string>("stepSize"));
					}
					else if (filterType == "MIN_NOTIONAL")
					{
						minValue = std::stod(filter.get<std::string>("minNotional"));
					}
				}

				pairs.emplace(std::move(pair), internal::binance_order_filters
					{
						static_cast<int>(-std::log10(tickSize)),
						static_cast<int>(-std::log10(qtyStepSize)),
						minQty,
						minValue
					});
			}

			return pairs;
		});
	}

	result<std::vector<ohlcv_data>> read_ohlcv(std::string_view jsonResult)
	{
		return read_result<std::vector<ohlcv_data>>(jsonResult, [](const json_document& json)
		{
			std::vector<ohlcv_data> data;
			data.reserve(json.size());

			for (auto it = json.begin(); it != json.end(); ++it)
			{
				json_element ohlcvElement{ it.value() };

				data.emplace(
					data.begin(),
					ohlcvElement.get<std::time_t>(0) / 1000,
					std::stod(ohlcvElement.get<std::string>(1)),
					std::stod(ohlcvElement.get<std::string>(2)),
					std::stod(ohlcvElement.get<std::string>(3)),
					std::stod(ohlcvElement.get<std::string>(4)),
					std::stod(ohlcvElement.get<std::string>(5)));
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

	result<std::unordered_map<std::string, double>> read_prices(std::string_view jsonResult)
	{
		return read_result<std::unordered_map<std::string, double>>(jsonResult, [](const json_document& json)
		{
			std::unordered_map<std::string, double> prices;
			prices.reserve(json.size());

			for (auto it = json.begin(); it != json.end(); ++it)
			{
				json_element priceElement{ it.value() };

				prices.emplace(
					priceElement.get<std::string>("symbol"), 
					std::stod(priceElement.get<std::string>("price")));
			}

			return prices;
		});
	}

	result<order_book_state> read_order_book(std::string_view jsonResult)
	{
		return read_result<order_book_state>(jsonResult, [](const json_document& json)
		{
			json_element asksElement{ json.element("asks") };
			json_element bidsElement{ json.element("bids") };

			std::vector<order_book_entry> asks;
			asks.reserve(asksElement.size());

			std::vector<order_book_entry> bids;
			bids.reserve(bidsElement.size());

			int size{ std::max<int>(asksElement.size(), bidsElement.size()) };
			auto asksIt = asksElement.begin();
			auto bidsIt = bidsElement.begin();

			for (int i = 0; i < size; ++i)
			{
				if (asksIt != asksElement.end())
				{
					asks.emplace_back(read_order_book_entry(asksIt, order_book_side::ASK));
					++asksIt;
				}

				if (bidsIt != bidsElement.end())
				{
					bids.emplace_back(read_order_book_entry(bidsIt, order_book_side::BID));
					++bidsIt;
				}
			}

			return order_book_state{ json.get<std::time_t>("lastUpdateId"), std::move(asks), std::move(bids)};
		});
	}

	result<double> read_fee(std::string_view jsonResult)
	{
		return read_result<double>(jsonResult, [](const json_document& json)
		{
			return json.get<double>("takerCommission") / 100;
		});
	}

	result<std::unordered_map<std::string, double>> read_balances(std::string_view jsonResult)
	{
		return read_result<std::unordered_map<std::string, double>>(jsonResult, [](const json_document& json)
		{
			json_element balancesElement{ json.element("balances") };
			std::unordered_map<std::string, double> balances;
			balances.reserve(balancesElement.size());

			for (auto it = balancesElement.begin(); it != balancesElement.end(); ++it)
			{
				json_element balanceElement{ it.value() };
				std::string asset{ balanceElement.get<std::string>("asset") };
				double balance{ std::stod(balanceElement.get<std::string>("free")) };

				balances.emplace(std::move(asset), balance);
			}

			return balances;
		});
	}

	result<std::vector<order_description>> read_open_orders(std::string_view jsonResult)
	{
		return read_result<std::vector<order_description>>(jsonResult, [](const json_document& json)
		{
			std::vector<order_description> orders;
			orders.reserve(json.size());
			
			for (auto it = json.begin(); it != json.end(); ++it)
			{
				orders.emplace_back(read_order_description(it.value()));
			}

			return orders;
		});
	}

	result<std::vector<order_description>> read_closed_orders(std::string_view jsonResult)
	{
		throw not_implemented_exception{ "binance::read_closed_orders" };
	}

	result<std::string> read_add_order(std::string_view jsonResult)
	{
		return read_result<std::string>(jsonResult, [](const json_document& json)
			{
				return std::to_string(json.get<long long>("orderId"));
			});
	}

	result<order_confirmation> read_add_order_confirm(std::string_view jsonResult)
	{
		return read_result<order_confirmation>(jsonResult, [](const json_document& json)
			{
				json_element fillsElement{ json.element("fills") };
				double avgPrice = 0.0;
				double filledQty = 0.0;

				for (auto it = fillsElement.begin(); it != fillsElement.end(); ++it)
				{
					json_element fill{ it.value() };
					double price{ std::stod(fill.get<std::string>("price")) };
					double qty{ std::stod(fill.get<std::string>("qty")) };
					avgPrice += price * qty;
					filledQty += qty;
				}

				avgPrice /= filledQty;

				return order_confirmation
				{
					std::to_string(json.get<long long>("orderId")),
					to_order_status(json.get<std::string>("status")),
					std::stod(json.get<std::string>("origQty")),
					std::stod(json.get<std::string>("executedQty")),
					avgPrice
				};
			});
	}

	result<void> read_cancel_order(std::string_view jsonResult)
	{
		return read_result<void>(jsonResult, [](const json_document& json)
			{
			});
	}
}
