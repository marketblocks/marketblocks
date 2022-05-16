#include <gtest/gtest.h>

#include "unittest/mocks.h"
#include "runner/backtest_runner.h"

namespace mb::test
{
	TEST(BacktestRunner, CorrectNumberOfIterationsAreRun)
	{
		constexpr int timeSteps = 10;

		back_testing_data backTestingData
		{
			{},
			{},
			0,
			1,
			timeSteps
		};

		auto dataNavigator = std::make_shared<back_testing_data_navigator>(backTestingData);
		auto marketApi = std::make_shared<backtest_market_api>(dataNavigator, nullptr);
		internal::backtest_runner<mock_strategy> runner{ marketApi, nullptr };

		mock_strategy mockStrategy{};
		EXPECT_CALL(mockStrategy, run_iteration)
			.Times(timeSteps);

		runner.run(mockStrategy);
	}

	TEST(BacktestRunner, EachIterationIsRunWithCorrectData)
	{
		constexpr int timeSteps = 3;

		back_testing_data backTestingData
		{
			{},
			{},
			0,
			1,
			timeSteps
		};

		auto dataNavigator = std::make_shared<back_testing_data_navigator>(backTestingData);
		auto marketApi = std::make_shared<backtest_market_api>(dataNavigator, nullptr);
		internal::backtest_runner<mock_strategy> runner{ marketApi, nullptr };

		mock_strategy mockStrategy{};
		EXPECT_CALL(mockStrategy, run_iteration)
			.WillOnce([dataNavigator]() { ASSERT_TRUE(dataNavigator->data_time() == 0); })
			.WillOnce([dataNavigator]() { ASSERT_TRUE(dataNavigator->data_time() == 1); })
			.WillOnce([dataNavigator]() { ASSERT_TRUE(dataNavigator->data_time() == 2); });

		runner.run(mockStrategy);
	}
}