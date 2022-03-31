#pragma once

#include <map>

#include "common/json/json.h"

namespace mb
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

	template<>
	fee_schedule from_json<fee_schedule>(const json_document& json);

	template<>
	void to_json<fee_schedule>(const fee_schedule& config, json_writer& writer);
}