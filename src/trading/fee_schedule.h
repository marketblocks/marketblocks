#pragma once

#include <map>

namespace cb
{
	class fee_schedule
	{
	private:
		std::map<double, double> _fees;

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
}