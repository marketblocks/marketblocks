#include "fee_schedule.h"

FeeSchedule::FeeSchedule(std::map<double, double> fees)
	: _fees{ std::move(fees) }
{}

double FeeSchedule::get_fee(double tradingVolume) const
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

FeeScheduleBuilder::FeeScheduleBuilder()
	: fees{}
{}

FeeScheduleBuilder FeeScheduleBuilder::add_tier(double tierUpperLimit, double fee)
{
	fees.emplace(tierUpperLimit, fee);

	return *this;
}

FeeSchedule FeeScheduleBuilder::build()
{
	return FeeSchedule{ fees };
}