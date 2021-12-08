#include "exchange_factories.h"
#include "kraken/kraken.h"
#include "paper_trading/paper_trader.h"

std::shared_ptr<Exchange> make_kraken()
{
	FeeSchedule fees = FeeScheduleBuilder{}
		.add_tier(1000, 0.26)
		.build();

	std::unordered_map<std::string, double> initialBalances
	{
		{ "GBP", 1000 }
	};

	return std::make_shared<Exchange>(
		std::make_unique<KrakenApi>(HttpService{}),
		std::make_unique<PaperTrader>(fees, initialBalances));
}