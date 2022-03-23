#pragma once

#include <map>

#include "common/json/json.h"

namespace cb
{
	class fee_schedule
	{
	private:
		std::map<double, double> _fees;

		friend void to_json<fee_schedule>(const fee_schedule& config, json_writer& writer);

	public:
		explicit fee_schedule(std::map<double, double> fees);

		double get_fee(double tradingVolume) const;
	};

	class fee_schedule_builder
	{
	private:
		std::map<double, double> fees;

	public:
		explicit fee_schedule_builder();

		fee_schedule_builder add_tier(double tierUpperLimit, double fee);

		fee_schedule build();
	};

	template<>
	fee_schedule from_json<fee_schedule>(const json_document& json);

	template<>
	void to_json<fee_schedule>(const fee_schedule& config, json_writer& writer);
}