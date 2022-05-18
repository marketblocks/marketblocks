#include <gtest/gtest.h>

#include "testing/back_testing/back_testing_report.h"

namespace mb::test
{
	TEST(BackTestingReport, GeneratingCorrectlyAddsBackTestingDataItems)
	{
		back_testing_data data
		{
			{},
			{},
			2,
			20,
			3,
			7
		};

		auto paperTradeApi = std::make_shared<paper_trade_api>(paper_trading_config{});

		back_testing_report report = generate_back_testing_report(data, {}, paperTradeApi, std::chrono::seconds{});

		EXPECT_EQ("01-01-1970 00:00:02", report.start_time());
		EXPECT_EQ("01-01-1970 00:00:20", report.end_time());
		EXPECT_EQ("3", report.step_size());
		EXPECT_EQ("7", report.time_steps());
	}

	TEST(BackTestingReport, GeneratingCorrectlyAddsTotalNumberOfTrades)
	{
		back_testing_data data { {}, {}, 0,	0, 0, 0	};

		auto paperTradeApi = std::make_shared<paper_trade_api>(paper_trading_config{0, { {"GBP", 1000} } });
		trade_description tradeDescription{ order_type::LIMIT, tradable_pair{"BTC", "GBP"}, trade_action::BUY, 0, 0 };
		paperTradeApi->add_order(tradeDescription);
		paperTradeApi->add_order(tradeDescription);
		paperTradeApi->add_order(tradeDescription);

		back_testing_report report = generate_back_testing_report(data, {}, paperTradeApi, std::chrono::seconds{});

		EXPECT_EQ("3", report.trades_count());
	}

	TEST(BackTestingReport, GeneratingReportCreatesCorrectAssetReports)
	{
		back_testing_data data{ {}, {}, 0, 63072000, 0, 0 };
		unordered_string_map<double> initialBalances{ {"GBP", 1000} };

		auto paperTradeApi = std::make_shared<paper_trade_api>(paper_trading_config{0, initialBalances});
		paperTradeApi->add_order(trade_description{ order_type::LIMIT, tradable_pair{"BTC", "GBP"}, trade_action::BUY, 100.0, 1.0 });

		back_testing_report report = generate_back_testing_report(data, initialBalances, paperTradeApi, std::chrono::seconds{});
		const std::vector<asset_report>& assetReports = report.asset_reports();

		ASSERT_EQ(2, assetReports.size());

		const asset_report& gbpReport = *std::find_if(assetReports.begin(), assetReports.end(), [](const asset_report& a) { return a.asset() == "GBP"; });
		const asset_report& btcReport = *std::find_if(assetReports.begin(), assetReports.end(), [](const asset_report& a) { return a.asset() == "BTC"; });

		EXPECT_EQ("1000.00", gbpReport.start_balance());
		EXPECT_EQ("900.00", gbpReport.end_balance());
		EXPECT_EQ("-100.00", gbpReport.change());
		EXPECT_EQ("-10.00%", gbpReport.percentage_change());
		EXPECT_EQ("-5.00%", gbpReport.annual_return());

		EXPECT_EQ("0.00", btcReport.start_balance());
		EXPECT_EQ("1.00", btcReport.end_balance());
		EXPECT_EQ("1.00", btcReport.change());
		EXPECT_EQ("N/A", btcReport.percentage_change());
		EXPECT_EQ("N/A", btcReport.annual_return());
	}
}