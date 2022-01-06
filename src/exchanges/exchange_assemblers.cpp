#include "exchange_assemblers.h"
#include "exchanges/paper_trading/paper_trader.h"

std::shared_ptr<Exchange> assemble_live(std::unique_ptr<Exchange> api)
{
	return api;
}

std::shared_ptr<Exchange> assemble_live_test(std::unique_ptr<Exchange> api)
{
	FeeSchedule fees = FeeScheduleBuilder{}
		.add_tier(1000, 0.26)
		.build();

	std::unordered_map<AssetSymbol, double> initialBalances
	{
		{ AssetSymbol { "GBP" }, 1000 }
	};

	return std::make_shared<LiveTestExchange>(
		api->id(),
		std::move(api),
		std::make_unique<PaperTrader>(std::move(fees), std::move(initialBalances)));
}