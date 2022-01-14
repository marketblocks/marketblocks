#include "fee_schedule.h"

namespace cb
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

	fee_schedule_builder::fee_schedule_builder()
		: fees{}
	{}

	fee_schedule_builder fee_schedule_builder::add_tier(double tierUpperLimit, double fee)
	{
		fees.emplace(tierUpperLimit, fee);

		return *this;
	}

	fee_schedule fee_schedule_builder::build()
	{
		return fee_schedule{ fees };
	}
}