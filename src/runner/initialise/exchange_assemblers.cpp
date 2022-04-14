#include "exchange_assemblers.h"

namespace mb
{
	std::shared_ptr<exchange> assemble_live::assemble(std::unique_ptr<exchange> api) const
	{
		return api;
	}

	assemble_live_test::assemble_live_test(paper_trading_config paperTradingConfig)
		: _paperTradingConfig{ std::move(paperTradingConfig) }
	{}

	std::shared_ptr<exchange> assemble_live_test::assemble(std::unique_ptr<exchange> api) const
	{
		return std::make_shared<live_test_exchange>(
			std::move(api),
			std::make_unique<paper_trade_api>(_paperTradingConfig));
	}

	assemble_back_test::assemble_back_test(back_testing_config backTestingConfig, paper_trading_config paperTradingConfig)
		: _backTestingConfig{ std::move(backTestingConfig) }, _paperTradingConfig{ std::move(paperTradingConfig) }
	{}

	std::shared_ptr<exchange> assemble_back_test::assemble(std::unique_ptr<exchange> api) const
	{
		return std::make_shared<back_test_exchange>(
			std::make_unique<backtest_market_api>(api->id()),
			std::make_unique<paper_trade_api>(_paperTradingConfig));
	}
}