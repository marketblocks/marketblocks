#include <unordered_set>

#include "digifinex_results.h"
#include "common/json/json.h"
#include "common/exceptions/not_implemented_exception.h"

namespace
{
	using namespace mb;

	std::string get_error_message(int code)
	{
		static std::unordered_map<int, std::string> errorMessages
		{
			{ 10001, "Wrong request method, please check it's a GET or POST request" },
			{ 10002, "Invalid ApiKey" },
			{ 10003, "Sign doesn't match" },
			{ 10004, "Illegal request parameters" },
			{ 10005, "Request frequency exceeds the limit" },
			{ 10006, "Unauthorized to execute this request" },
			{ 10007, "IP address Unauthorized" },
			{ 10008, "Timestamp for this request is invalid" },
			{ 10009, "Endpoint does not exist or missing ACCESS-KEY, please check endpoint URL" },
			{ 10011, "ApiKey expired. Please go to client side to re-create an ApiKey" },
			{ 20002, "Trade of this trading pair is suspended" },
			{ 20007, "Price precision error" },
			{ 20008, "Amount precision error" },
			{ 20009, "Amount is less than the minimum requirement" },
			{ 20010, "Cash Amount is less than the minimum requirement" },
			{ 20011, "Insufficient balance" },
			{ 20012, "Invalid trade type (valid value: buy/sell)" },
			{ 20013, "No order info found" },
			{ 20014, "Invalid date (Valid format: 2018-07-25)" },
			{ 20015, "Date exceeds the limit" },
			{ 20018, "Your have been banned for API trading by the system" },
			{ 20019, "Wrong trading pair symbol, correct format:\"base_quote\", e.g. \"btc_usdt\"" },
			{ 20020, "You have violated the API trading rules and temporarily banned for trading. At present, we have certain restrictions on the user's transaction rate and withdrawal rate." },
			{ 20021, "Invalid currency" },
			{ 20022, "The ending timestamp must be larger than the starting timestamp" },
			{ 20023, "Invalid transfer type" },
			{ 20024, "Invalid amount" },
			{ 20025, "This currency is not transferable at the moment" },
			{ 20026, "Transfer amount exceed your balance" },
			{ 20027, "Abnormal account status" },
			{ 20028, "Blacklist for transfer" },
			{ 20029, "Transfer amount exceed your daily limit" },
			{ 20030, "You have no position on this trading pair" },
			{ 20032, "Withdrawal limited" },
			{ 20033, "Wrong Withdrawal ID" },
			{ 20034, "Withdrawal service of this crypto has been closed" },
			{ 20035, "Withdrawal limit" },
			{ 20036, "Withdrawal cancellation failed" },
			{ 20037, "The withdrawal address, Tag or chain type is not included in the withdrawal management list" },
			{ 20038, "The withdrawal address is not on the white list" },
			{ 20039, "Can't be canceled in current status" },
			{ 20040, "Withdraw too frequently; limitation: 3 times a minute, 100 times a day" },
			{ 20041, "Beyond the daily withdrawal limit" },
			{ 20042, "Current trading pair does not support API trading" },
			{ 50000, "Exception error" },
			{ -99999, "This is an error" },
		};

		auto it = errorMessages.find(code);
		if (it == errorMessages.end())
		{
			return "Unknown error occured. Error code: " + std::to_string(code);
		}

		return it->second;
	}

	template<typename T, typename Reader>
	result<T> read_result(std::string_view jsonResult, std::string_view resultName, const Reader& reader)
	{
		try
		{
			json_document jsonDocument{ parse_json(jsonResult) };

			int errorCode = jsonDocument.get<int>("code");
			if (errorCode != 0)
			{
				return result<T>::fail(get_error_message(errorCode));
			}

			return result<T>::success(reader(jsonDocument.element(resultName)));
		}
		catch (const std::exception& e)
		{
			return result<T>::fail(e.what());
		}
	}
}

namespace mb::digifinex
{
	result<exchange_status> read_system_status(std::string_view jsonResult)
	{
		return read_result<exchange_status>(jsonResult, "msg", [](const json_element& resultElement)
		{
			return exchange_status::ONLINE;
		});
	}

	result<std::vector<tradable_pair>> read_tradable_pairs(std::string_view jsonResult)
	{
		return read_result<std::vector<tradable_pair>>(jsonResult, "symbol_list", [](const json_element& resultElement)
		{
			std::unordered_set<tradable_pair> pairs;
			pairs.reserve(resultElement.size());

			for (auto it = resultElement.begin(); it != resultElement.end(); ++it)
			{
				json_element pairElement{ it.value() };

				std::string asset{ pairElement.get<std::string>("base_asset") };
				std::string priceUnit{ pairElement.get<std::string>("quote_asset") };

				pairs.emplace(std::move(asset), std::move(priceUnit));
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
		return read_result<double>(jsonResult, "ticker", [](const json_element& resultElement)
		{
			json_element tickerElement{ resultElement.begin().value() };
			return tickerElement.get<double>("last");
		});
	}

	result<order_book_state> read_order_book(std::string_view jsonResult)
	{
		throw not_implemented_exception{ "digifinex::read_order_book" };
	}

	result<unordered_string_map<double>> read_balances(std::string_view jsonResult)
	{
		return read_result<unordered_string_map<double>>(jsonResult, "list", [](const json_element& resultElement)
		{
			unordered_string_map<double> balances;
			balances.reserve(resultElement.size());

			for (auto it = resultElement.begin(); it != resultElement.end(); ++it)
			{
				json_element balanceElement{ it.value() };

				std::string asset{ balanceElement.get<std::string>("currency") };
				double balance{ balanceElement.get<double>("free") };

				balances.emplace(std::move(asset), std::move(balance));
			}

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
		return read_result<std::string>(jsonResult, "order_id", [](const json_element& resultElement)
		{
			return resultElement.get<std::string>();
		});
	}

	result<void> read_cancel_order(std::string_view jsonResult)
	{
		throw not_implemented_exception{ "digifinex::read_cancel_order" };
	}
}