#include "paper_trading_config.h"

namespace
{
	namespace json_property_names
	{
		static constexpr std::string_view FEE = "fee";
		static constexpr std::string_view BALANCES = "balances";
	}
}

namespace mb
{
	paper_trading_config::paper_trading_config()
		: _fee{}, _balances{}
	{}

	paper_trading_config::paper_trading_config(double fee, std::unordered_map<std::string,double> balances)
		: _fee{ fee }, _balances{ std::move(balances) }
	{}

	template<>
	paper_trading_config from_json<paper_trading_config>(const json_document& json)
	{
		double fee{ json.get<double>(json_property_names::FEE) };
		std::unordered_map<std::string,double> balances{ json.get<std::unordered_map<std::string,double>>(json_property_names::BALANCES) };

		return paper_trading_config{ std::move(fee), std::move(balances) };
	}

	template<>
	void to_json<paper_trading_config>(const paper_trading_config& config, json_writer& writer)
	{
		writer.add(json_property_names::FEE, config.fee());
		writer.add(json_property_names::BALANCES, config.balances());
	}
}