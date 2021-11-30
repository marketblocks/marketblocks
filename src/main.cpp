﻿#include <iostream>

#include "runner.h"
#include "exchanges/exchange.h"
#include "exchanges/kraken/kraken.h"
#include "exchanges/paper_trader/paper_trader.h"
#include "strategies/tri_arb.h"
#include "networking/httpservice.h"

int main()
{
	std::vector<Exchange> exchanges;
	std::unordered_map<std::string, double> initialBalance
	{
		{"GBP", 1000.0}
	};
	exchanges.emplace_back(std::make_unique<KrakenMarketData>(), std::make_unique<PaperTrader>(initialBalance));

	TriArbStrategy triArb = TriArbStrategy::create(exchanges);

	CryptoBot::runner::run(triArb);

	return 0;
}