#include <iostream>

#include "runner.h"
#include "exchanges/exchange.h"
#include "exchanges/dummy_exchange.h"
#include "strategies/test_strategy.h"
#include "strategies/tri_arb.h"

int main()
{
	std::vector<std::shared_ptr<Exchange>> exchanges;
	exchanges.emplace_back(std::make_shared<DummyExchange>());

	TestStrategy s;
	TriArbStrategy triArb = TriArbStrategy::create(exchanges);

	CryptoBot::runner::run(triArb);

	return 0;
}
