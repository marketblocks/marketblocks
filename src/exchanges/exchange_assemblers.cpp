#include "exchange_assemblers.h"

namespace cb
{
	std::shared_ptr<exchange> assemble_live(std::unique_ptr<exchange> api)
	{
		return api;
	}

	std::shared_ptr<exchange> assemble_live_test(std::unique_ptr<exchange> api, paper_trading_config paperTradingConfig)
	{
		return std::make_shared<live_test_exchange>(
			std::move(api),
			std::make_unique<paper_trader>(std::move(paperTradingConfig)));
	}
}