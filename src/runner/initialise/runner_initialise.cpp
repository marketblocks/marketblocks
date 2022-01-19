#include <stdexcept>
#include <functional>

#include "runner_initialise.h"
#include "initialisation_error.h"
#include "exchanges/exchange_id.h"
#include "exchanges/kraken/kraken.h"
#include "exchanges/exchange_assemblers.h"
#include "common/file/file.h"
#include "common/exceptions/cb_exception.h"
#include "logging/logger.h"
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

	void log_exchange_list(const std::vector<std::string>& exchangeIds, logger& log)
	{
		std::string exchangeList;

		for (auto& id : exchangeIds)
		{
			exchangeList += "\n" + id;
		}

		log.info("Found {0} exchange(s):{1}", exchangeIds.size(), exchangeList);
	}

	std::vector<std::shared_ptr<cb::exchange>> create_exchanges(
		const std::vector<std::string>& exchangeIds, 
		const cb::exchange_id_lookup& idLookup, 
		const exchange_assembler& assembler,
		std::shared_ptr<cb::websocket_client> websocketClient)
	{
		std::vector<std::shared_ptr<cb::exchange>> exchanges;
		exchanges.reserve(exchangeIds.size());

		logger& log{ logger::instance() };
		log_exchange_list(exchangeIds, log);

		for (auto& exchangeId : exchangeIds)
		{
			log.info("Creating exchange API: {}", exchangeId);

			std::unique_ptr<cb::exchange> api = create_api_from_id(exchangeId, idLookup, websocketClient);

			if (!api)
			{
				logger::instance().warning("Exchange '{}' is not supported", exchangeId);
				continue;
			}

			log.info("{} API created successfully", exchangeId);
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

		logger::instance().info("Creating exchange APIs...");

		if (runnerConfig.exchange_ids().empty())
		{
			logger::instance().warning("No exchanges specified, using all supported exchanges");
			return ::create_exchanges(idLookup.all_ids(), idLookup, assembler, websocketClient);
		}
		else
		{
			return ::create_exchanges(runnerConfig.exchange_ids(), idLookup, assembler, websocketClient);
		}
	}
}