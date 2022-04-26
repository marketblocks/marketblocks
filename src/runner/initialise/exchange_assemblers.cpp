#include "exchange_assemblers.h"
#include "common/csv/csv.h"
#include "testing/paper_trading/paper_trade_api.h"
#include "testing/back_testing/backtest_market_api.h"
#include "testing/back_testing/data_loading.h"

namespace mb
{
	std::shared_ptr<exchange> assemble_live::assemble(std::shared_ptr<exchange> api) const
	{
		return api;
	}

	assemble_live_test::assemble_live_test(paper_trading_config paperTradingConfig)
		: _paperTradingConfig{ std::move(paperTradingConfig) }
	{}

	std::shared_ptr<exchange> assemble_live_test::assemble(std::shared_ptr<exchange> api) const
	{
		return std::make_shared<live_test_exchange>(
			std::move(api),
			std::make_unique<paper_trade_api>(_paperTradingConfig));
	}

	std::shared_ptr<exchange> create_back_test_exchange(back_testing_config backTestingConfig, paper_trading_config paperTradingConfig)
	{
		back_testing_data data{ load_back_testing_data(backTestingConfig) };
		std::unique_ptr<backtest_websocket_stream> websocketStream{ std::make_unique<backtest_websocket_stream>() };

		return std::make_shared<back_test_exchange>(
			std::make_unique<backtest_market_api>(std::move(data), std::move(websocketStream)),
			std::make_unique<paper_trade_api>(paperTradingConfig));
	}
}