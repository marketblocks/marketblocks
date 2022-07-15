#include "binance_results.h"
#include "common/json/json.h"

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

	result<std::vector<tradable_pair>> read_tradable_pairs(std::string_view jsonResult)
	{
		return read_result<std::vector<tradable_pair>>(jsonResult, [](const json_document& json)
		{
			json_element symbolsElement{ json.element("symbols") };
			std::vector<tradable_pair> pairs;
			pairs.reserve(symbolsElement.size());

			for (auto it = symbolsElement.begin(); it != symbolsElement.end(); ++it)
			{
				json_element pairElement{ it.value() };

				std::string asset{ pairElement.get<std::string>("baseAsset") };
				std::string priceUnit{ pairElement.get<std::string>("quoteAsset") };
				
				pairs.emplace_back(std::move(asset), std::move(priceUnit));
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

			return order_book_state{ std::move(asks), std::move(bids) };
		});
	}

	result<double> read_fee(std::string_view jsonResult)
	{
		return read_result<double>(jsonResult, [](const json_document& json)
		{
			json_element feeElement{ json.begin().value() };
			return std::stod(feeElement.get<std::string>("takerCommission")) * 100;
		});
	}

	result<unordered_string_map<double>> read_balances(std::string_view jsonResult)
	{
		return result<unordered_string_map<double>>::success({});
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