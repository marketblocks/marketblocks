#include <stdexcept>
#include <functional>

#include "runner_initialise.h"
#include "initialisation_error.h"
#include "exchanges/exchange_id.h"
#include "exchanges/kraken/kraken.h"
#include "exchanges/exchange_assemblers.h"
#include "exchanges/exchange_status.h"
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
		std::string_view identifier,
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

			log.info("{} API created successfully. Testing connection...", exchangeId);

			exchange_status status = api->get_status();
			std::string status_string = to_string(status);

			if (status == exchange_status::ONLINE)
			{
				log.info("Exchange status: {}", status_string);
			}
			else
			{
				log.warning("Exchange status: {}", status_string);
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

	std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig, run_mode runMode)
	{
		std::vector<std::shared_ptr<exchange>> exchanges;
		std::shared_ptr<websocket_client> websocketClient = std::make_shared<websocket_client>();
		exchange_assembler assembler = select_assembler(runMode);
		exchange_id_lookup idLookup;

		logger::instance().info("Creating exchange APIs...");

		if (runnerConfig.exchange_ids().empty())
		{
			return ::create_exchanges(idLookup.all_ids(), idLookup, assembler, websocketClient);
		}
		else
		{
			return ::create_exchanges(runnerConfig.exchange_ids(), idLookup, assembler, websocketClient);
		}
	}
}