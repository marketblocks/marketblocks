#include <iostream>

#include "runner.h"
#include "exchanges/exchange.h"
#include "exchanges/kraken/kraken.h"
#include "strategies/tri_arb.h"
#include "networking/httpservice.h"

int main()
{
	std::vector<std::shared_ptr<Exchange>> exchanges;
	exchanges.emplace_back(std::make_shared<KrakenExchange>());

	TriArbStrategy triArb = TriArbStrategy::create(exchanges);

	CryptoBot::runner::run(triArb);

	return 0;
}
