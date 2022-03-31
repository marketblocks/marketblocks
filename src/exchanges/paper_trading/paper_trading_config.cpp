#include "paper_trading_config.h"

namespace
{
	namespace json_property_names
	{
		static constexpr std::string_view FEE_SCHEDULE = "feeSchedule";
		static constexpr std::string_view BALANCES = "balances";
	}
}

namespace mb
{
	paper_trading_config::paper_trading_config()
		: _fees{ std::map<double, double>{} }, _balances{}
	{}

	paper_trading_config::paper_trading_config(fee_schedule fees, unordered_string_map<double> balances)
		: _fees{ std::move(fees) }, _balances{ std::move(balances) }
	{}

	template<>
	paper_trading_config from_json<paper_trading_config>(const json_document& json)
	{
		fee_schedule fees{ from_json<fee_schedule>(json.element(json_property_names::FEE_SCHEDULE).to_string()) };
		unordered_string_map<double> balances{ json.get<unordered_string_map<double>>(json_property_names::BALANCES) };

		return paper_trading_config{ std::move(fees), std::move(balances) };
	}

	template<>
	void to_json<paper_trading_config>(const paper_trading_config& config, json_writer& writer)
	{
		writer.add(json_property_names::FEE_SCHEDULE, to_json(config.fees()));
		writer.add(json_property_names::BALANCES, config.balances());
	}
}