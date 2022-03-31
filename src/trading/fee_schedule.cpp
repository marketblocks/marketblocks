#include "fee_schedule.h"

namespace
{
	namespace json_property_names
	{
		static constexpr std::string_view FEES = "fees";
	}
}

namespace mb
{
	fee_schedule::fee_schedule(std::map<double, double> fees)
		: _fees{ std::move(fees) }
	{}

	double fee_schedule::get_fee(double tradingVolume) const
	{
		for (auto& [tierLimit, fee] : _fees)
		{
			if (tradingVolume <= tierLimit)
			{
				return fee;
			}
		}

		return 0.0;
	}

	template<>
	fee_schedule from_json<fee_schedule>(const json_document& json)
	{
		return fee_schedule(json.get<std::map<double, double>>(json_property_names::FEES));
	}

	template<>
	void to_json<fee_schedule>(const fee_schedule& config, json_writer& writer)
	{
		writer.add(json_property_names::FEES, config._fees);
	}
}