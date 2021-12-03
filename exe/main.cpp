#include <iostream>

#include "runner.h"
#include "exchanges/exchange.h"
#include "exchanges/exchange_factories.h"
#include "strategies/tri_arb.h"

int main()
{
	std::vector<std::shared_ptr<Exchange>> exchanges;
	exchanges.emplace_back(make_kraken());

	TradingOptions options{ 0.05 };

	TriArbStrategy triArb = create_tri_arb_strategy(exchanges, options);

	CryptoBot::runner::run(triArb);

	return 0;
}
