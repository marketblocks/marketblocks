#include <gtest/gtest.h>

#include "trading/fee_schedule.h"

namespace cb::test
{
	TEST(fee_schedule, BuilderAddsTiersInOrder)
	{
		fee_schedule_builder builder;

		fee_schedule feeSchedule = builder
			.add_tier(1000, 0.5)
			.add_tier(10000, 0.1)
			.add_tier(5000, 0.3)
			.build();

		EXPECT_DOUBLE_EQ(feeSchedule.get_fee(2500), 0.3);
	}
}