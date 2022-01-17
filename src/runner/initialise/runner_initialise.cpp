#include <stdexcept>
#include <functional>

#include "runner_initialise.h"
#include "initialisation_error.h"
#include "exchange_factories.h"
#include "exchanges/exchange_id.h"
#include "exchanges/kraken/kraken.h"
#include "exchanges/exchange_assemblers.h"
#include "common/file/file.h"
#include "common/exceptions/cb_exception.h"
#include "networking/websocket/websocket_client.h"

namespace
{
	typedef std::shared_ptr<cb::exchange>(*exchange_assembler)(std::unique_ptr<cb::exchange> api);
	exchange_assembler select_assembler(cb::run_mode runMode)
	{
		switch (runMode)
		{
			case cb::run_mode::LIVE:
				return cb::assemble_live;
			case cb::run_mode::LIVETEST:
				return cb::assemble_live_test;
			default:
				throw std::invalid_argument("Invalid run mode");
		}
	}

	std::unique_ptr<cb::exchange> create_api_from_id(const std::string& identifier, const cb::exchange_id_lookup& idLookup, std::shared_ptr<cb::websocket_client> websocketClient)
	{
		auto it = idLookup.map().find(identifier);
		if (it == idLookup.map().end())
		{
			throw std::invalid_argument("Exchange identifier: " + identifier + "not recognised");
		}

		cb::exchange_id id = it->second;

		switch (id)
		{
			case cb::exchange_id::KRAKEN:
				return cb::make_kraken(websocketClient);
		}
	}
}

namespace cb::internal
{
	runner_config get_runner_config()
	{
		try
		{
			runner_config config = load_or_create_config<runner_config>();
			return config;
		}
		catch (const cb_exception& e)
		{
			throw initialisation_error{ std::format("Error occured reading runner config: {}", e.what()) };
		}
	}

	trading_options get_trading_options()
	{
		try
		{
			trading_options options = load_or_create_config<trading_options>();
			return options;
		}
		catch (const cb_exception& e)
		{
			throw initialisation_error{ std::format("Error occured reading trading options: {}", e.what()) };
		}
	}

	std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig, run_mode runMode)
	{
		std::vector<std::shared_ptr<exchange>> exchanges;
		std::shared_ptr<websocket_client> websocketClient = std::make_shared<websocket_client>();
		exchange_assembler assembler = select_assembler(runMode);
		exchange_id_lookup idLookup;

		for (auto& exchangeId : runnerConfig.exchange_ids())
		{
			std::shared_ptr<exchange> exchange = assembler(create_api_from_id(exchangeId, idLookup, websocketClient));
			exchange->get_or_connect_websocket();

			exchanges.push_back(exchange);
		}

		return exchanges;
	}
}