#pragma once

#include <map>

class FeeSchedule
{
private:
	std::map<double, double> _fees;

public:
	explicit FeeSchedule(std::map<double, double> fees);

	double get_fee(double tradingVolume) const;
};

class FeeScheduleBuilder
{
private:
	std::map<double, double> fees;

public:
	explicit FeeScheduleBuilder();

	FeeScheduleBuilder add_tier(double tierUpperLimit, double fee);

	FeeSchedule build();
};