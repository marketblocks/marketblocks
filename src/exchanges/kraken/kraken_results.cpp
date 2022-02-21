#include "kraken_results.h"
#include "common/file/json.h"
#include "common/utils/stringutils.h"
#include "logging/logger.h"

namespace
{
	std::string get_error(cb::json_document& json)
	{
		std::vector<std::string> messages = json.get<std::vector<std::string>>("error");
		
		return messages.empty()
			? ""
			: messages[0];
	}

	template<typename T, typename Reader> 
	cb::result<T> read_result(const std::string& jsonResult, const Reader& reader)
	{
		cb::json_document jsonDocument{ cb::parse_json(jsonResult) };

		std::string error = get_error(jsonDocument);
		if (!error.empty())
		{
			return cb::result<T>::fail(std::move(error));
		}

		return reader(jsonDocument.element("result"));
	}
}

namespace cb::internal
{
	result<exchange_status> read_system_status(const std::string& jsonResult)
	{
		return read_result<exchange_status>(jsonResult, [](const json_element& resultElement)
		{
			std::string status_string{ resultElement.get<std::string>("status") };
			exchange_status status;

			if (status_string == "online")
			{
				status = exchange_status::ONLINE;
			}
			else if (status_string == "cancel_only")
			{
				status = exchange_status::CANCEL_ONLY;
			}
			else if (status_string == "post_only")
			{
				status = exchange_status::POST_ONLY;
			}
			else
			{
				status = exchange_status::MAINTENANCE;
			}

			return result<exchange_status>::success(std::move(status));
		});
	}

	result<std::vector<tradable_pair>> read_tradable_pairs(const std::string& jsonResult)
	{
		return read_result<std::vector<tradable_pair>>(jsonResult, [](const json_element& resultElement)
		{
			std::vector<tradable_pair> pairs;
			pairs.reserve(resultElement.size());

			for (auto it = resultElement.begin(); it != resultElement.end(); ++it)
			{
				std::string name{ it.key() };
				std::string wsName{ it.value().get<std::string>("wsname") };
				std::vector<std::string> assetSymbols{ split(wsName, '/') };
				pairs.emplace_back(name, asset_symbol{ assetSymbols[0] }, asset_symbol{ assetSymbols[1] });
			}

			return result<std::vector<tradable_pair>>::success(std::move(pairs));
		});
	}

	result<ticker_data> read_ticker_data(const std::string& jsonResult)
	{
		return read_result<ticker_data>(jsonResult, [](const json_element& resultElement)
		{
			json_element dataElement{ resultElement.begin().value() };

			std::vector<std::string> asks{ dataElement.get<std::vector<std::string>>("a") };
			std::vector<std::string> bids{ dataElement.get<std::vector<std::string>>("b") };
			std::vector<std::string> volumes{ dataElement.get<std::vector<std::string>>("v") };
			std::vector<int> trades{ dataElement.get<std::vector<int>>("t") };
			std::vector<std::string> lows{ dataElement.get<std::vector<std::string>>("l") };
			std::vector<std::string> highs{ dataElement.get<std::vector<std::string>>("h") };
			std::string openingPrice{ dataElement.get<std::string>("o") };

			return result<ticker_data>::success(ticker_data
			{
				std::stod(asks[0]),
				std::stod(asks[2]),
				std::stod(bids[0]),
				std::stod(bids[2]),
				std::stod(volumes[0]),
				std::stod(volumes[1]),
				trades[0],
				trades[1],
				std::stod(lows[0]),
				std::stod(lows[1]),
				std::stod(highs[0]),
				std::stod(highs[1]),
				std::stod(openingPrice)
			});
		});
	}

	result<order_book_state> read_order_book(const std::string& jsonResult)
	{
		return read_result<order_book_state>(jsonResult, [](const json_element& resultElement)
		{
			json_element bookElement{ resultElement.begin().value() };

			json_element asks{ bookElement.element("asks") };
			json_element bids{ bookElement.element("bids") };

			int depth = asks.size();
			std::vector<order_book_level> levels;
			levels.reserve(depth);

			for (int i = 0; i < depth; i++)
			{
				json_element asks_i{ asks.element(i) };
				order_book_entry askEntry
				{
					order_book_side::ASK,
					std::stod(asks_i.element(0).get<std::string>()),
					std::stod(asks_i.element(1).get<std::string>()),
					asks_i.element(2).get<double>()
				};

				json_element bids_i{ bids.element(i) };
				order_book_entry bidEntry
				{
					order_book_side::BID,
					std::stod(bids_i.element(0).get<std::string>()),
					std::stod(bids_i.element(1).get<std::string>()),
					bids_i.element(2).get<double>()
				};

				levels.emplace_back(std::move(askEntry), std::move(bidEntry));
			}

			return result<order_book_state>::success(std::move(levels));
		});
	}

	result<std::unordered_map<asset_symbol, double>> read_balances(const std::string& jsonResult)
	{
		return read_result<std::unordered_map<asset_symbol, double>>(jsonResult, [](const json_element& resultElement)
		{
			std::unordered_map<asset_symbol, double> balances;
			balances.reserve(resultElement.size());

			for (auto it = resultElement.begin(); it != resultElement.end(); ++it)
			{
				balances.emplace(asset_symbol{ it.key() }, std::stod(it.value().get<std::string>()));
			}

			return result<std::unordered_map<asset_symbol, double>>::success(std::move(balances));
		});
	}
}