#include "exchange_factories.h"
#include "kraken/kraken.h"
#include "paper_trading/paper_trader.h"

std::shared_ptr<Exchange> make_kraken()
{
	return std::make_shared<Exchange>(std::make_unique<KrakenMarketData>(), std::make_unique<PaperTrader>(0.26));
}