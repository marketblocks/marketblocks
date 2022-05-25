#include "back_test_runner.h"
#include "config_file_reader.h"
#include "testing/back_testing/data_loading.h"
#include "common/utils/timeutils.h"

namespace mb::internal
{
	std::shared_ptr<backtest_market_api> create_backtest_market_api(const back_testing_config& config)
	{
		back_testing_data data = load_back_testing_data(config);
		std::shared_ptr<back_testing_data_navigator> dataNavigator = std::make_shared<back_testing_data_navigator>(std::move(data));

		std::unique_ptr<backtest_websocket_stream> websocketStream{ std::make_unique<backtest_websocket_stream>(dataNavigator) };
		return std::make_shared<backtest_market_api>(dataNavigator, std::move(websocketStream));
	}

	std::shared_ptr<paper_trade_api> create_paper_trade_api()
	{
		paper_trading_config config = load_or_create_config<paper_trading_config>();

		return std::make_unique<paper_trade_api>(std::move(config));
	}
}