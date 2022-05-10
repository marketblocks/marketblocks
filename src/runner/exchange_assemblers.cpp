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
}