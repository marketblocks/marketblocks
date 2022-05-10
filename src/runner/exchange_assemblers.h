#pragma once

#include <memory>

#include "exchanges/exchange.h"
#include "testing/paper_trading/paper_trading_config.h"
#include "testing/back_testing/back_testing_config.h"

namespace mb
{
	class exchange_assembler
	{
	public:
		virtual std::shared_ptr<exchange> assemble(std::shared_ptr<exchange> api) const = 0;
	};

	class assemble_live : public exchange_assembler
	{
	public:
		std::shared_ptr<exchange> assemble(std::shared_ptr<exchange> api) const override;
	};

	class assemble_live_test : public exchange_assembler
	{
	private:
		paper_trading_config _paperTradingConfig;

	public:
		assemble_live_test(paper_trading_config paperTradingConfig);

		std::shared_ptr<exchange> assemble(std::shared_ptr<exchange> api) const override;
	};
}