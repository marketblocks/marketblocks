#include <iostream>

#include "runner.h"
#include "strategies/test_strategy.h"
#include "strategies/tri_arb.h"

int main()
{
	TestStrategy s;
	TriArbStrategy triArb = TriArbStrategy::create();

	CryptoBot::runner::run(triArb);

	return 0;
}
