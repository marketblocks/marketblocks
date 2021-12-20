#include "exchange_factories.h"
#include "kraken/kraken.h"
#include "paper_trading/paper_trader.h"

std::shared_ptr<Exchange> make_kraken()
{
	FeeSchedule fees = FeeScheduleBuilder{}
		.add_tier(1000, 0.26)
		.build();

	std::unordered_map<AssetSymbol, double> initialBalances
	{
		{ AssetSymbol { "GBP" }, 1000 }
	};

	return std::make_shared<MultiComponentExchange<KrakenApi, PaperTrader>>(
		KrakenApi{ HttpService{} },
		PaperTrader{ fees, initialBalances });
}
