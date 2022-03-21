#include "exchange_assemblers.h"
#include "exchanges/paper_trading/paper_trader.h"

namespace cb
{
	std::shared_ptr<exchange> assemble_live(std::unique_ptr<exchange> api)
	{
		return api;
	}

	std::shared_ptr<exchange> assemble_live_test(std::unique_ptr<exchange> api)
	{
		fee_schedule fees = fee_schedule_builder{}
			.add_tier(1000, 0.26)
			.build();

		unordered_string_map<double> initialBalances
		{
			{ "GBP", 1000 },
			{ "EUR", 1000 },
			{ "USD", 1000 }
		};

		return std::make_shared<live_test_exchange>(
			std::move(api),
			std::make_unique<paper_trader>(std::move(fees), std::move(initialBalances)));
	}
}