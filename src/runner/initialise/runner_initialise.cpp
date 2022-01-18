#include <stdexcept>
#include <functional>

#include "runner_initialise.h"
#include "initialisation_error.h"
#include "exchanges/exchange_id.h"
#include "exchanges/kraken/kraken.h"
#include "exchanges/exchange_assemblers.h"
#include "common/file/file.h"
#include "common/exceptions/cb_exception.h"
#include "networking/websocket/websocket_client.h"

namespace
{
	using namespace cb;

	typedef std::shared_ptr<exchange>(*exchange_assembler)(std::unique_ptr<exchange> api);
	exchange_assembler select_assembler(run_mode runMode)
	{
		switch (runMode)
		{
			case run_mode::LIVE:
				return assemble_live;
			case run_mode::LIVETEST:
				return assemble_live_test;
			default:
				throw initialisation_error{ "Run mode not supported" };
		}
	}

	std::unique_ptr<exchange> create_api_from_id(
		const std::string& identifier,
		const exchange_id_lookup& idLookup,
		std::shared_ptr<websocket_client> websocketClient)
	{
		auto it = idLookup.map().find(identifier);
		if (it == idLookup.map().end())
		{
			return nullptr;
		}

		exchange_id id = it->second;

		switch (id)
		{
			case exchange_id::KRAKEN:
			{
				kraken_config config = internal::get_config<kraken_config>();
				return make_kraken(std::move(config), websocketClient);
			}
			default:
				return nullptr;
		}
	}

	std::vector<std::shared_ptr<cb::exchange>> create_exchanges(
		const std::vector<std::string>& exchangeIds, 
		const cb::exchange_id_lookup& idLookup, 
		const exchange_assembler& assembler,
		std::shared_ptr<cb::websocket_client> websocketClient)
	{
		std::vector<std::shared_ptr<cb::exchange>> exchanges;
		exchanges.reserve(exchangeIds.size());

		for (auto& exchangeId : exchangeIds)
		{
			std::unique_ptr<cb::exchange> api = create_api_from_id(exchangeId, idLookup, websocketClient);

			if (!api)
			{
				// log
				continue;
			}

			exchanges.emplace_back(assembler(std::move(api)));
		}

		return exchanges;
	}
}

namespace cb::internal
{
	runner_config get_runner_config()
	{
		return get_config<runner_config>();
	}

	trading_options get_trading_options()
	{
		return get_config<trading_options>();
	}

	std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig, run_mode runMode)
	{
		std::vector<std::shared_ptr<exchange>> exchanges;
		std::shared_ptr<websocket_client> websocketClient = std::make_shared<websocket_client>();
		exchange_assembler assembler = select_assembler(runMode);
		exchange_id_lookup idLookup;

		if (runnerConfig.exchange_ids().empty())
		{
			// log
			return ::create_exchanges(idLookup.all_ids(), idLookup, assembler, websocketClient);
		}
		else
		{
			return ::create_exchanges(runnerConfig.exchange_ids(), idLookup, assembler, websocketClient);
		}
	}
}